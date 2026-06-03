#!/usr/bin/env python3
"""
Benchmark Engine de GraphRush.

Componentes:
- BenchmarkRunner
- MetricsCollector
- CSVReporter
- JSONReporter
- MarkdownReporter

La metodología por defecto usa 4 ejecuciones y reporta el promedio de las últimas 3,
siguiendo una práctica inspirada en GAP Benchmark Suite.
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
from statistics import mean


TIME_RE = re.compile(r"Tiempo:\s+([0-9]+(?:\.[0-9]+)?)\s+ms")
EDGES_RE = re.compile(r"Aristas:\s+([0-9]+)")
MEMORY_RE = re.compile(r"Maximum resident set size \(kbytes\):\s+([0-9]+)")
CACHE_MISSES_RE = re.compile(r"([0-9,]+)\s+cache-misses")
CACHE_REFERENCES_RE = re.compile(r"([0-9,]+)\s+cache-references")


@dataclass
class BenchmarkMetric:
    algorithm: str
    threads: int
    runs: int
    reported_runs: int
    total_time_ms: float
    average_time_ms: float
    speedup: float
    parallel_efficiency: float
    edges_per_second: float
    gteps: float
    memory_peak_kb: int | None
    cache_misses: int | None
    cache_references: int | None
    command: str


class MetricsCollector:
    """Extrae métricas desde la salida de GraphRush, /usr/bin/time y perf."""

    @staticmethod
    def extract_time_ms(output: str, fallback_ms: float) -> float:
        match = TIME_RE.search(output)
        if match:
            return float(match.group(1))
        return fallback_ms

    @staticmethod
    def extract_memory_peak_kb(output: str) -> int | None:
        match = MEMORY_RE.search(output)
        if not match:
            return None
        return int(match.group(1))

    @staticmethod
    def extract_cache_misses(output: str) -> int | None:
        match = CACHE_MISSES_RE.search(output)
        if not match:
            return None
        return int(match.group(1).replace(",", ""))

    @staticmethod
    def extract_cache_references(output: str) -> int | None:
        match = CACHE_REFERENCES_RE.search(output)
        if not match:
            return None
        return int(match.group(1).replace(",", ""))

    @staticmethod
    def extract_edges(output: str) -> int:
        match = EDGES_RE.search(output)
        if not match:
            return 0
        return int(match.group(1))


class BenchmarkRunner:
    """Ejecuta GraphRush con múltiples algoritmos, hilos y repeticiones."""

    def __init__(
        self,
        binary: str,
        graph: str,
        algorithms: list[str],
        threads: list[int],
        source: int,
        iterations: int,
        top_k: int,
        runs: int,
        warmup: int,
        use_perf: bool,
        measure_memory: bool,
    ) -> None:
        self.binary = binary
        self.graph = graph
        self.algorithms = algorithms
        self.threads = threads
        self.source = source
        self.iterations = iterations
        self.top_k = top_k
        self.runs = runs
        self.warmup = warmup
        self.use_perf = use_perf
        self.measure_memory = measure_memory
        self.collector = MetricsCollector()

    def read_edge_count(self) -> int:
        output, _, _ = self.run_raw([self.binary, "stats", "--graph", self.graph])
        return self.collector.extract_edges(output)

    def build_command(self, algorithm: str, threads: int) -> list[str]:
        if algorithm == "bfs":
            return [
                self.binary,
                "bfs",
                "--graph",
                self.graph,
                "--source",
                str(self.source),
                "--threads",
                str(threads),
            ]

        if algorithm == "components":
            return [
                self.binary,
                "components",
                "--graph",
                self.graph,
                "--threads",
                str(threads),
            ]

        if algorithm == "pagerank":
            return [
                self.binary,
                "pagerank",
                "--graph",
                self.graph,
                "--iterations",
                str(self.iterations),
                "--threads",
                str(threads),
                "--top-k",
                str(self.top_k),
            ]

        raise ValueError(f"Algoritmo no soportado: {algorithm}")

    def wrap_command(self, command: list[str]) -> list[str]:
        wrapped = command

        if self.use_perf and shutil.which("perf"):
            wrapped = [
                "perf",
                "stat",
                "-e",
                "cache-references,cache-misses",
                *wrapped,
            ]

        if self.measure_memory and Path("/usr/bin/time").exists():
            wrapped = ["/usr/bin/time", "-v", *wrapped]

        return wrapped

    def run_raw(self, command: list[str]) -> tuple[str, float, int | None]:
        wrapped = self.wrap_command(command)
        start = time.perf_counter()
        completed = subprocess.run(
            wrapped,
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )
        wall_ms = (time.perf_counter() - start) * 1000.0
        memory_peak_kb = self.collector.extract_memory_peak_kb(completed.stdout)
        return completed.stdout, wall_ms, memory_peak_kb

    def run_once(self, command: list[str]) -> tuple[float, int | None, int | None, int | None]:
        output, wall_ms, memory_peak_kb = self.run_raw(command)
        elapsed_ms = self.collector.extract_time_ms(output, wall_ms)
        cache_misses = self.collector.extract_cache_misses(output)
        cache_references = self.collector.extract_cache_references(output)
        return elapsed_ms, memory_peak_kb, cache_misses, cache_references

    def run(self) -> list[BenchmarkMetric]:
        edges = self.read_edge_count()
        metrics: list[BenchmarkMetric] = []

        for algorithm in self.algorithms:
            baseline_time: float | None = None

            for thread_count in self.threads:
                command = self.build_command(algorithm, thread_count)

                for _ in range(self.warmup):
                    self.run_once(command)

                times: list[float] = []
                memory_values: list[int] = []
                cache_miss_values: list[int] = []
                cache_reference_values: list[int] = []

                for _ in range(self.runs):
                    elapsed_ms, memory_peak_kb, cache_misses, cache_references = self.run_once(command)
                    times.append(elapsed_ms)

                    if memory_peak_kb is not None:
                        memory_values.append(memory_peak_kb)
                    if cache_misses is not None:
                        cache_miss_values.append(cache_misses)
                    if cache_references is not None:
                        cache_reference_values.append(cache_references)

                reported = times[-3:] if len(times) >= 4 else times
                average_time = mean(reported)
                total_time = sum(times)

                if baseline_time is None:
                    baseline_time = average_time

                speedup = baseline_time / average_time if average_time > 0 else 0.0
                efficiency = speedup / float(thread_count) if thread_count > 0 else 0.0
                edges_per_second = float(edges) / (average_time / 1000.0) if average_time > 0 else 0.0
                gteps = edges_per_second / 1_000_000_000.0

                metrics.append(
                    BenchmarkMetric(
                        algorithm=algorithm,
                        threads=thread_count,
                        runs=self.runs,
                        reported_runs=len(reported),
                        total_time_ms=total_time,
                        average_time_ms=average_time,
                        speedup=speedup,
                        parallel_efficiency=efficiency,
                        edges_per_second=edges_per_second,
                        gteps=gteps,
                        memory_peak_kb=max(memory_values) if memory_values else None,
                        cache_misses=max(cache_miss_values) if cache_miss_values else None,
                        cache_references=max(cache_reference_values) if cache_reference_values else None,
                        command=" ".join(command),
                    )
                )

        return metrics


class CSVReporter:
    def write(self, metrics: list[BenchmarkMetric], path: Path) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)
        if not metrics:
            raise RuntimeError("No hay métricas para escribir CSV.")

        with path.open("w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=list(asdict(metrics[0]).keys()))
            writer.writeheader()
            for metric in metrics:
                writer.writerow(asdict(metric))


class JSONReporter:
    def write(self, metrics: list[BenchmarkMetric], path: Path) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)
        with path.open("w", encoding="utf-8") as f:
            json.dump([asdict(metric) for metric in metrics], f, indent=2)


class MarkdownReporter:
    def write(self, metrics: list[BenchmarkMetric], path: Path) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)

        with path.open("w", encoding="utf-8") as f:
            f.write("### Resultados del Benchmark Engine\n\n")
            f.write("#### Metodología\n\n")
            f.write("Se ejecutan varias repeticiones por configuración. Si hay 4 o más runs, ")
            f.write("se reporta el promedio de las últimas 3 ejecuciones.\n\n")
            f.write("#### Tabla de resultados\n\n")
            f.write("| Algoritmo | Hilos | Tiempo prom. ms | Speedup | Eficiencia | Aristas/s | GTEPS | Memoria KB | Cache misses |\n")
            f.write("|---|---:|---:|---:|---:|---:|---:|---:|---:|\n")

            for metric in metrics:
                memory = "" if metric.memory_peak_kb is None else str(metric.memory_peak_kb)
                cache = "" if metric.cache_misses is None else str(metric.cache_misses)
                f.write(
                    f"| {metric.algorithm} | {metric.threads} | "
                    f"{metric.average_time_ms:.4f} | {metric.speedup:.4f} | "
                    f"{metric.parallel_efficiency:.4f} | {metric.edges_per_second:.2f} | "
                    f"{metric.gteps:.8f} | {memory} | {cache} |\n"
                )


class PerfSummaryReporter:
    def write(self, metrics: list[BenchmarkMetric], path: Path, use_perf: bool) -> None:
        path.parent.mkdir(parents=True, exist_ok=True)

        with path.open("w", encoding="utf-8") as f:
            f.write("### Resumen perf\n\n")
            if not use_perf:
                f.write("#### Estado\n\n")
                f.write("`perf` no fue activado. Ejecute con `--use-perf` para medir cache misses.\n")
                return

            f.write("#### Cache misses\n\n")
            f.write("| Algoritmo | Hilos | Cache references | Cache misses |\n")
            f.write("|---|---:|---:|---:|\n")

            for metric in metrics:
                references = "" if metric.cache_references is None else str(metric.cache_references)
                misses = "" if metric.cache_misses is None else str(metric.cache_misses)
                f.write(f"| {metric.algorithm} | {metric.threads} | {references} | {misses} |\n")


def parse_list(value: str) -> list[str]:
    return [item.strip() for item in value.split(",") if item.strip()]


def parse_int_list(value: str) -> list[int]:
    return [int(item.strip()) for item in value.split(",") if item.strip()]


def main() -> None:
    parser = argparse.ArgumentParser(description="Benchmark Engine profesional de GraphRush.")
    parser.add_argument("--binary", default="graphrush", help="Ruta al binario graphrush.")
    parser.add_argument("--graph", required=True, help="Grafo binario .grcsr/.gr.")
    parser.add_argument("--algos", default="bfs,pagerank,components")
    parser.add_argument("--threads", default="1,2,4,8,16")
    parser.add_argument("--source", type=int, default=0)
    parser.add_argument("--iterations", type=int, default=20)
    parser.add_argument("--top-k", type=int, default=20)
    parser.add_argument("--runs", type=int, default=4)
    parser.add_argument("--warmup", type=int, default=1)
    parser.add_argument("--output", default="reports")
    parser.add_argument("--use-perf", action="store_true")
    parser.add_argument(
        "--measure-memory",
        dest="measure_memory",
        action="store_true",
        help="Medir memoria pico con /usr/bin/time -v.",
    )

    parser.add_argument(
        "--no-measure-memory",
        dest="measure_memory",
        action="store_false",
        help="Desactivar medición de memoria pico.",
    )

    parser.set_defaults(measure_memory=True)
    args = parser.parse_args()

    runner = BenchmarkRunner(
        binary=args.binary,
        graph=args.graph,
        algorithms=parse_list(args.algos),
        threads=parse_int_list(args.threads),
        source=args.source,
        iterations=args.iterations,
        top_k=args.top_k,
        runs=args.runs,
        warmup=args.warmup,
        use_perf=args.use_perf,
        measure_memory=args.measure_memory,
    )

    metrics = runner.run()
    output_dir = Path(args.output)

    CSVReporter().write(metrics, output_dir / "benchmark.csv")
    JSONReporter().write(metrics, output_dir / "benchmark.json")
    MarkdownReporter().write(metrics, output_dir / "benchmark_results.md")
    PerfSummaryReporter().write(metrics, output_dir / "perf_summary.md", args.use_perf)

    print("[GraphRush] Benchmark Engine completado correctamente.")
    print(f"[GraphRush] Reportes guardados en: {output_dir}")


if __name__ == "__main__":
    main()
