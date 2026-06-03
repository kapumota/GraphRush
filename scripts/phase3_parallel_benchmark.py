#!/usr/bin/env python3
"""
Ejecuta benchmarks de Fase 3.1/3.2 para GraphRush.

Genera:
- reports/phase3/parallel_benchmark.csv
- reports/phase3/parallel_benchmark.json
- reports/phase3/parallel_benchmark.md

Fase 3.2 agrega medición de memoria pico con /usr/bin/time -v.
"""

from __future__ import annotations

import argparse
import csv
import json
import re
import shutil
import subprocess
import time
from dataclasses import asdict, dataclass
from pathlib import Path


TIME_RE = re.compile(r"Tiempo:\s+([0-9]+(?:\.[0-9]+)?)\s+ms")
EDGES_RE = re.compile(r"Aristas:\s+([0-9]+)")
MEMORY_RE = re.compile(r"Maximum resident set size \(kbytes\):\s+([0-9]+)")


@dataclass
class BenchmarkRow:
    algorithm: str
    threads: int
    elapsed_ms: float
    speedup: float
    efficiency: float
    edges_per_second: float
    memory_peak_kb: int | None
    command: str


def parse_threads(value: str) -> list[int]:
    return [int(item.strip()) for item in value.split(",") if item.strip()]


def has_usr_bin_time() -> bool:
    return Path("/usr/bin/time").exists()


def run_command(command: list[str], measure_memory: bool) -> tuple[str, float, int | None]:
    full_command = command
    if measure_memory and has_usr_bin_time():
        full_command = ["/usr/bin/time", "-v", *command]

    start = time.perf_counter()
    completed = subprocess.run(
        full_command,
        check=True,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    elapsed_ms = (time.perf_counter() - start) * 1000.0
    memory_peak_kb = extract_memory_peak_kb(completed.stdout)

    return completed.stdout, elapsed_ms, memory_peak_kb


def extract_time(output: str, fallback_ms: float) -> float:
    match = TIME_RE.search(output)
    if match:
        return float(match.group(1))
    return fallback_ms


def extract_memory_peak_kb(output: str) -> int | None:
    match = MEMORY_RE.search(output)
    if not match:
        return None
    return int(match.group(1))


def read_edge_count(binary: str, graph: str) -> int:
    output, _, _ = run_command([binary, "stats", "--graph", graph], measure_memory=False)
    match = EDGES_RE.search(output)
    return int(match.group(1)) if match else 0


def build_command(
    binary: str,
    graph: str,
    algorithm: str,
    threads: int,
    source: int,
    iterations: int,
    top_k: int,
) -> list[str]:
    if algorithm == "bfs":
        return [
            binary,
            "bfs",
            "--graph",
            graph,
            "--source",
            str(source),
            "--threads",
            str(threads),
        ]

    if algorithm == "components":
        return [
            binary,
            "components",
            "--graph",
            graph,
            "--threads",
            str(threads),
        ]

    if algorithm == "pagerank":
        return [
            binary,
            "pagerank",
            "--graph",
            graph,
            "--iterations",
            str(iterations),
            "--threads",
            str(threads),
            "--top-k",
            str(top_k),
        ]

    raise ValueError(f"Algoritmo no soportado: {algorithm}")


def run_benchmark(args: argparse.Namespace) -> list[BenchmarkRow]:
    algorithms = [item.strip() for item in args.algos.split(",") if item.strip()]
    threads_values = parse_threads(args.threads)
    edges = read_edge_count(args.binary, args.graph)
    rows: list[BenchmarkRow] = []

    if args.measure_memory and not has_usr_bin_time():
        print("[GraphRush] Advertencia: /usr/bin/time no está disponible; memory_peak_kb quedará vacío.")

    for algorithm in algorithms:
        baseline_ms: float | None = None

        for threads in threads_values:
            command = build_command(
                binary=args.binary,
                graph=args.graph,
                algorithm=algorithm,
                threads=threads,
                source=args.source,
                iterations=args.iterations,
                top_k=args.top_k,
            )

            output, wall_ms, memory_peak_kb = run_command(
                command,
                measure_memory=args.measure_memory,
            )
            elapsed_ms = extract_time(output, wall_ms)

            if baseline_ms is None:
                baseline_ms = elapsed_ms

            speedup = (baseline_ms / elapsed_ms) if elapsed_ms > 0 else 0.0
            efficiency = speedup / float(threads) if threads > 0 else 0.0
            edges_per_second = float(edges) / (elapsed_ms / 1000.0) if elapsed_ms > 0 else 0.0

            rows.append(
                BenchmarkRow(
                    algorithm=algorithm,
                    threads=threads,
                    elapsed_ms=elapsed_ms,
                    speedup=speedup,
                    efficiency=efficiency,
                    edges_per_second=edges_per_second,
                    memory_peak_kb=memory_peak_kb,
                    command=" ".join(command),
                )
            )

    return rows


def write_csv(rows: list[BenchmarkRow], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    if not rows:
        raise RuntimeError("No hay filas para escribir en CSV.")

    with path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=list(asdict(rows[0]).keys()))
        writer.writeheader()
        for row in rows:
            writer.writerow(asdict(row))


def write_json(rows: list[BenchmarkRow], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as f:
        json.dump([asdict(row) for row in rows], f, indent=2)


def write_markdown(rows: list[BenchmarkRow], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)

    with path.open("w", encoding="utf-8") as f:
        f.write("### Benchmark paralelo - Fase 3.2\n\n")
        f.write("#### Resultados\n\n")
        f.write("| Algoritmo | Hilos | Tiempo ms | Speedup | Eficiencia | Aristas/s | Memoria pico KB |\n")
        f.write("|---|---:|---:|---:|---:|---:|---:|\n")

        for row in rows:
            memory = "" if row.memory_peak_kb is None else str(row.memory_peak_kb)
            f.write(
                f"| {row.algorithm} | {row.threads} | {row.elapsed_ms:.4f} | "
                f"{row.speedup:.4f} | {row.efficiency:.4f} | "
                f"{row.edges_per_second:.2f} | {memory} |\n"
            )


def main() -> None:
    parser = argparse.ArgumentParser(description="Benchmark paralelo de GraphRush.")
    parser.add_argument("--binary", default="graphrush")
    parser.add_argument("--graph", required=True)
    parser.add_argument("--algos", default="bfs,pagerank,components")
    parser.add_argument("--threads", default="1,2,4,8,16")
    parser.add_argument("--source", type=int, default=0)
    parser.add_argument("--iterations", type=int, default=20)
    parser.add_argument("--top-k", type=int, default=20)
    parser.add_argument("--output", default="reports/phase3")
    parser.add_argument(
        "--measure-memory",
        action=argparse.BooleanOptionalAction,
        default=True,
        help="Activa o desactiva medición de memoria pico con /usr/bin/time -v.",
    )
    args = parser.parse_args()

    rows = run_benchmark(args)
    output_dir = Path(args.output)

    write_csv(rows, output_dir / "parallel_benchmark.csv")
    write_json(rows, output_dir / "parallel_benchmark.json")
    write_markdown(rows, output_dir / "parallel_benchmark.md")

    print("[GraphRush] Benchmark paralelo completado correctamente.")
    print(f"[GraphRush] Reportes guardados en: {output_dir}")


if __name__ == "__main__":
    main()
