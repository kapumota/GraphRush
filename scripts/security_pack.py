#!/usr/bin/env python3
"""
GraphRush Security Pack.

Fase 6:
- import
- stats
- rank
- path
- components
- report

Fase 6.1:
- export-csr
- core-rank
- core-path
- core-sssp

La Fase 6.1 mapea nodos simbólicos a IDs numéricos y ejecuta PageRank/BFS/SSSP
directamente sobre el core C++ de GraphRush mediante el binario de la CLI.
"""

from __future__ import annotations

import argparse
import csv
import heapq
import json
import re
import subprocess
from collections import defaultdict, deque
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import Any


RANK_RE = re.compile(r"rank=([0-9]+)\s+node=([0-9]+)\s+score=([0-9.eE+-]+)")
DISTANCE_RE = re.compile(r"Distancia(?: máxima)?:\s+([0-9.eE+-]+)")


@dataclass
class SecurityEdge:
    source: str
    target: str
    relation: str
    weight: float


@dataclass
class SecurityGraph:
    nodes: list[str]
    edges: list[SecurityEdge]


def load_csv(path: Path) -> SecurityGraph:
    edges: list[SecurityEdge] = []
    nodes: set[str] = set()

    with path.open("r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        required = {"source", "target", "relation", "weight"}
        missing = required.difference(reader.fieldnames or [])
        if missing:
            raise RuntimeError(f"Faltan columnas requeridas: {sorted(missing)}")

        for row in reader:
            source = row["source"].strip()
            target = row["target"].strip()
            relation = row["relation"].strip()
            weight_text = row.get("weight", "1").strip() or "1"

            if not source or not target:
                continue

            weight = float(weight_text)
            if weight < 0:
                raise RuntimeError("No se aceptan pesos negativos en grafos de seguridad.")

            edges.append(SecurityEdge(source, target, relation, weight))
            nodes.add(source)
            nodes.add(target)

    return SecurityGraph(nodes=sorted(nodes), edges=edges)


def save_graph(graph: SecurityGraph, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "schema_version": "graphrush.security.v1",
        "nodes": graph.nodes,
        "edges": [asdict(edge) for edge in graph.edges],
    }
    with path.open("w", encoding="utf-8") as f:
        json.dump(payload, f, indent=2)


def load_graph(path: Path) -> SecurityGraph:
    with path.open("r", encoding="utf-8") as f:
        payload = json.load(f)
    edges = [SecurityEdge(**edge) for edge in payload["edges"]]
    return SecurityGraph(nodes=list(payload["nodes"]), edges=edges)


def build_adjacency(graph: SecurityGraph, undirected: bool = False) -> dict[str, list[tuple[str, str, float]]]:
    adjacency: dict[str, list[tuple[str, str, float]]] = defaultdict(list)
    for edge in graph.edges:
        adjacency[edge.source].append((edge.target, edge.relation, edge.weight))
        if undirected:
            adjacency[edge.target].append((edge.source, edge.relation, edge.weight))
    for node in graph.nodes:
        adjacency.setdefault(node, [])
    return adjacency


def build_mapping(graph: SecurityGraph) -> tuple[dict[str, int], dict[int, str]]:
    node_to_id = {node: index for index, node in enumerate(sorted(graph.nodes))}
    id_to_node = {index: node for node, index in node_to_id.items()}
    return node_to_id, id_to_node


def write_mapping(node_to_id: dict[str, int], id_to_node: dict[int, str], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = {
        "schema_version": "graphrush.security.mapping.v1",
        "node_to_id": node_to_id,
        "id_to_node": {str(k): v for k, v in id_to_node.items()},
    }
    with path.open("w", encoding="utf-8") as f:
        json.dump(payload, f, indent=2)


def load_mapping(path: Path) -> tuple[dict[str, int], dict[int, str]]:
    with path.open("r", encoding="utf-8") as f:
        payload = json.load(f)
    node_to_id = {str(k): int(v) for k, v in payload["node_to_id"].items()}
    id_to_node = {int(k): str(v) for k, v in payload["id_to_node"].items()}
    return node_to_id, id_to_node


def export_weighted_edgelist(graph: SecurityGraph, edge_list_path: Path, mapping_path: Path) -> None:
    node_to_id, id_to_node = build_mapping(graph)

    edge_list_path.parent.mkdir(parents=True, exist_ok=True)
    with edge_list_path.open("w", encoding="utf-8") as f:
        for edge in graph.edges:
            f.write(f"{node_to_id[edge.source]} {node_to_id[edge.target]} {edge.weight}\n")

    write_mapping(node_to_id, id_to_node, mapping_path)


def run_command(command: list[str]) -> str:
    completed = subprocess.run(
        command,
        check=True,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    return completed.stdout


def import_numeric_csr(binary: str, edge_list: Path, output_csr: Path) -> str:
    output_csr.parent.mkdir(parents=True, exist_ok=True)
    return run_command(
        [
            binary,
            "import",
            "--input",
            str(edge_list),
            "--format",
            "snap",
            "--output",
            str(output_csr),
            "--directed",
            "--deduplicate",
            "--weighted",
        ]
    )


def ensure_core_artifacts(
    graph_path: Path,
    output_dir: Path,
    binary: str,
) -> tuple[SecurityGraph, Path, Path, Path, dict[str, int], dict[int, str]]:
    graph = load_graph(graph_path)
    output_dir.mkdir(parents=True, exist_ok=True)

    edge_list = output_dir / "security_numeric.edges"
    mapping = output_dir / "security_mapping.json"
    csr = output_dir / "security_numeric.grcsr"

    export_weighted_edgelist(graph, edge_list, mapping)
    import_numeric_csr(binary, edge_list, csr)

    node_to_id, id_to_node = load_mapping(mapping)
    return graph, edge_list, mapping, csr, node_to_id, id_to_node


def parse_rank_output(output: str, id_to_node: dict[int, str]) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    for match in RANK_RE.finditer(output):
        rank = int(match.group(1))
        node_id = int(match.group(2))
        score = float(match.group(3))
        rows.append(
            {
                "rank": rank,
                "node_id": node_id,
                "node": id_to_node.get(node_id, str(node_id)),
                "score": score,
            }
        )
    return rows


def read_distance_csv(path: Path) -> dict[int, float]:
    distances: dict[int, float] = {}
    with path.open("r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            value = row["distance"]
            distances[int(row["node"])] = float("inf") if value == "inf" else float(value)
    return distances


def command_import(args: argparse.Namespace) -> None:
    graph = load_csv(Path(args.input))
    save_graph(graph, Path(args.output))
    print("[GraphRush Security] Grafo de seguridad importado correctamente.")
    print(f"[GraphRush Security] Nodos: {len(graph.nodes)}")
    print(f"[GraphRush Security] Relaciones: {len(graph.edges)}")
    print(f"[GraphRush Security] Salida: {args.output}")


def command_stats(args: argparse.Namespace) -> None:
    graph = load_graph(Path(args.graph))
    relation_counts: dict[str, int] = defaultdict(int)
    out_degree: dict[str, int] = defaultdict(int)
    in_degree: dict[str, int] = defaultdict(int)

    for edge in graph.edges:
        relation_counts[edge.relation] += 1
        out_degree[edge.source] += 1
        in_degree[edge.target] += 1

    summary = {
        "nodes": len(graph.nodes),
        "edges": len(graph.edges),
        "relation_types": dict(sorted(relation_counts.items())),
        "max_out_degree": max(out_degree.values(), default=0),
        "max_in_degree": max(in_degree.values(), default=0),
    }

    if args.json:
        print(json.dumps(summary, indent=2))
        return

    print("[GraphRush Security] Estadísticas del grafo.")
    print(f"[GraphRush Security] Nodos: {summary['nodes']}")
    print(f"[GraphRush Security] Relaciones: {summary['edges']}")
    print(f"[GraphRush Security] Grado de salida máximo: {summary['max_out_degree']}")
    print(f"[GraphRush Security] Grado de entrada máximo: {summary['max_in_degree']}")
    print("[GraphRush Security] Tipos de relación:")
    for relation, count in summary["relation_types"].items():
        print(f"  - {relation}: {count}")


def pagerank(graph: SecurityGraph, iterations: int, damping: float) -> dict[str, float]:
    adjacency = build_adjacency(graph)
    nodes = graph.nodes
    n = len(nodes)
    if n == 0:
        return {}

    scores = {node: 1.0 / n for node in nodes}
    for _ in range(iterations):
        next_scores = {node: (1.0 - damping) / n for node in nodes}
        dangling_mass = 0.0
        for node in nodes:
            outgoing = adjacency[node]
            if not outgoing:
                dangling_mass += scores[node]
                continue
            contribution = damping * scores[node] / len(outgoing)
            for target, _, _ in outgoing:
                next_scores[target] += contribution
        dangling_contribution = damping * dangling_mass / n
        for node in nodes:
            next_scores[node] += dangling_contribution
        scores = next_scores
    return scores


def command_rank(args: argparse.Namespace) -> None:
    graph = load_graph(Path(args.graph))
    if args.method != "pagerank":
        raise RuntimeError("Por ahora solo se soporta --method pagerank.")
    scores = pagerank(graph, args.iterations, args.damping)
    ranked = sorted(scores.items(), key=lambda item: (-item[1], item[0]))[: args.top]

    if args.output_csv:
        output = Path(args.output_csv)
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open("w", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            writer.writerow(["rank", "node", "score"])
            for index, (node, score) in enumerate(ranked, start=1):
                writer.writerow([index, node, f"{score:.10f}"])

    print("[GraphRush Security] Nodos críticos por PageRank.")
    for index, (node, score) in enumerate(ranked, start=1):
        print(f"[GraphRush Security] rank={index} node={node} score={score:.10f}")


def bfs_path(graph: SecurityGraph, source: str, target: str) -> list[str]:
    adjacency = build_adjacency(graph)
    queue: deque[str] = deque([source])
    parent: dict[str, str | None] = {source: None}
    while queue:
        current = queue.popleft()
        if current == target:
            break
        for neighbor, _, _ in adjacency[current]:
            if neighbor not in parent:
                parent[neighbor] = current
                queue.append(neighbor)
    if target not in parent:
        return []
    path: list[str] = []
    current: str | None = target
    while current is not None:
        path.append(current)
        current = parent[current]
    path.reverse()
    return path


def dijkstra_path(graph: SecurityGraph, source: str, target: str) -> tuple[list[str], float]:
    adjacency = build_adjacency(graph)
    distances: dict[str, float] = {node: float("inf") for node in graph.nodes}
    parent: dict[str, str | None] = {source: None}
    distances[source] = 0.0
    heap: list[tuple[float, str]] = [(0.0, source)]
    while heap:
        distance, current = heapq.heappop(heap)
        if distance != distances[current]:
            continue
        if current == target:
            break
        for neighbor, _, weight in adjacency[current]:
            candidate = distance + weight
            if candidate < distances[neighbor]:
                distances[neighbor] = candidate
                parent[neighbor] = current
                heapq.heappush(heap, (candidate, neighbor))
    if target not in parent:
        return [], float("inf")
    path: list[str] = []
    current: str | None = target
    while current is not None:
        path.append(current)
        current = parent[current]
    path.reverse()
    return path, distances[target]


def command_path(args: argparse.Namespace) -> None:
    graph = load_graph(Path(args.graph))
    if args.source not in graph.nodes:
        raise RuntimeError(f"No existe el nodo fuente: {args.source}")
    if args.target not in graph.nodes:
        raise RuntimeError(f"No existe el nodo destino: {args.target}")

    if args.algo == "bfs":
        path = bfs_path(graph, args.source, args.target)
        distance = len(path) - 1 if path else None
    elif args.algo in {"dijkstra", "sssp"}:
        path, distance = dijkstra_path(graph, args.source, args.target)
    else:
        raise RuntimeError("Algoritmo no soportado. Use bfs, dijkstra o sssp.")

    payload = {
        "source": args.source,
        "target": args.target,
        "algo": args.algo,
        "path": path,
        "distance": distance,
        "reachable": bool(path),
    }

    if args.output_json:
        output = Path(args.output_json)
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open("w", encoding="utf-8") as f:
            json.dump(payload, f, indent=2)

    print("[GraphRush Security] Camino de ataque.")
    if path:
        print("[GraphRush Security] " + " -> ".join(path))
        print(f"[GraphRush Security] Distancia: {distance}")
    else:
        print("[GraphRush Security] No se encontró camino.")


def compute_components(graph: SecurityGraph) -> list[list[str]]:
    adjacency = build_adjacency(graph, undirected=True)
    visited: set[str] = set()
    components: list[list[str]] = []
    for node in graph.nodes:
        if node in visited:
            continue
        queue = deque([node])
        visited.add(node)
        component: list[str] = []
        while queue:
            current = queue.popleft()
            component.append(current)
            for neighbor, _, _ in adjacency[current]:
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append(neighbor)
        components.append(sorted(component))
    components.sort(key=lambda item: (-len(item), item))
    return components


def command_components(args: argparse.Namespace) -> None:
    graph = load_graph(Path(args.graph))
    components = [component for component in compute_components(graph) if len(component) >= args.min_size]

    if args.output_json:
        output = Path(args.output_json)
        output.parent.mkdir(parents=True, exist_ok=True)
        with output.open("w", encoding="utf-8") as f:
            json.dump(
                [{"id": index, "size": len(component), "nodes": component} for index, component in enumerate(components)],
                f,
                indent=2,
            )

    print("[GraphRush Security] Componentes detectadas.")
    for index, component in enumerate(components):
        print(f"[GraphRush Security] component={index} size={len(component)}")


def command_report(args: argparse.Namespace) -> None:
    graph = load_graph(Path(args.graph))
    output = Path(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)

    critical_nodes_path = output.parent / "critical_nodes.csv"
    attack_paths_path = output.parent / "attack_paths.json"
    components_path = output.parent / "components.json"
    summary_path = output.parent / "security_summary.md"

    scores = pagerank(graph, args.iterations, 0.85)
    ranked = sorted(scores.items(), key=lambda item: (-item[1], item[0]))[: args.top]

    with critical_nodes_path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["rank", "node", "score"])
        for index, (node, score) in enumerate(ranked, start=1):
            writer.writerow([index, node, f"{score:.10f}"])

    components = compute_components(graph)
    with components_path.open("w", encoding="utf-8") as f:
        json.dump(
            [{"id": index, "size": len(component), "nodes": component} for index, component in enumerate(components)],
            f,
            indent=2,
        )

    attack_payload: dict[str, Any] = {"paths": []}
    if args.source and args.target:
        path = bfs_path(graph, args.source, args.target)
        attack_payload["paths"].append(
            {"source": args.source, "target": args.target, "algo": "bfs", "path": path, "reachable": bool(path)}
        )
    with attack_paths_path.open("w", encoding="utf-8") as f:
        json.dump(attack_payload, f, indent=2)

    with output.open("w", encoding="utf-8") as f:
        f.write("### GraphRush Security Report\n\n")
        f.write("#### Resumen\n\n")
        f.write(f"- Nodos: {len(graph.nodes)}\n")
        f.write(f"- Relaciones: {len(graph.edges)}\n")
        f.write(f"- Componentes: {len(components)}\n\n")
        f.write("#### Nodos críticos\n\n")
        f.write("| Rank | Nodo | Score |\n")
        f.write("|---:|---|---:|\n")
        for index, (node, score) in enumerate(ranked, start=1):
            f.write(f"| {index} | {node} | {score:.10f} |\n")
        f.write("\n#### Archivos generados\n\n")
        f.write(f"- `{critical_nodes_path}`\n")
        f.write(f"- `{attack_paths_path}`\n")
        f.write(f"- `{components_path}`\n")
        f.write(f"- `{summary_path}`\n")

    with summary_path.open("w", encoding="utf-8") as f:
        f.write("### Resumen ejecutivo de seguridad\n\n")
        f.write("#### Hallazgos\n\n")
        f.write(f"- Nodos analizados: {len(graph.nodes)}\n")
        f.write(f"- Relaciones analizadas: {len(graph.edges)}\n")
        f.write(f"- Componentes conectadas: {len(components)}\n")
        if ranked:
            f.write(f"- Nodo más crítico por PageRank: {ranked[0][0]}\n")

    print("[GraphRush Security] Reporte generado correctamente.")
    print(f"[GraphRush Security] Reporte principal: {output}")


def command_export_csr(args: argparse.Namespace) -> None:
    graph = load_graph(Path(args.graph))
    edge_list = Path(args.edge_list)
    mapping = Path(args.mapping)
    output_csr = Path(args.output_csr)

    export_weighted_edgelist(graph, edge_list, mapping)
    import_output = import_numeric_csr(args.binary, edge_list, output_csr)

    print("[GraphRush Security Core] Exportación simbólica -> CSR completada.")
    print(f"[GraphRush Security Core] EdgeList numérico: {edge_list}")
    print(f"[GraphRush Security Core] Mapping: {mapping}")
    print(f"[GraphRush Security Core] CSR ponderado: {output_csr}")
    print(import_output)


def command_core_rank(args: argparse.Namespace) -> None:
    _, _, mapping, csr, _, id_to_node = ensure_core_artifacts(
        Path(args.graph),
        Path(args.output_dir),
        args.binary,
    )

    output = run_command(
        [
            args.binary,
            "pagerank",
            "--graph",
            str(csr),
            "--iterations",
            str(args.iterations),
            "--threads",
            str(args.threads),
            "--top-k",
            str(args.top),
        ]
    )

    ranked = parse_rank_output(output, id_to_node)
    csv_path = Path(args.output_dir) / "core_critical_nodes.csv"
    md_path = Path(args.output_dir) / "core_rank.md"
    json_path = Path(args.output_dir) / "core_rank.json"

    with csv_path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["rank", "node_id", "node", "score"])
        writer.writeheader()
        writer.writerows(ranked)

    with json_path.open("w", encoding="utf-8") as f:
        json.dump({"mapping": str(mapping), "ranked": ranked, "raw_output": output}, f, indent=2)

    with md_path.open("w", encoding="utf-8") as f:
        f.write("### Security Core Rank\n\n")
        f.write("#### PageRank ejecutado en core C++\n\n")
        f.write("| Rank | ID | Nodo | Score |\n")
        f.write("|---:|---:|---|---:|\n")
        for row in ranked:
            f.write(f"| {row['rank']} | {row['node_id']} | {row['node']} | {row['score']:.10f} |\n")

    print("[GraphRush Security Core] PageRank ejecutado en core C++.")
    print(f"[GraphRush Security Core] CSV: {csv_path}")
    print(f"[GraphRush Security Core] Markdown: {md_path}")


def command_core_path(args: argparse.Namespace) -> None:
    graph, _, _, csr, node_to_id, id_to_node = ensure_core_artifacts(
        Path(args.graph),
        Path(args.output_dir),
        args.binary,
    )

    if args.source not in node_to_id:
        raise RuntimeError(f"No existe el nodo fuente: {args.source}")
    if args.target not in node_to_id:
        raise RuntimeError(f"No existe el nodo destino: {args.target}")

    source_id = node_to_id[args.source]
    target_id = node_to_id[args.target]
    distances_csv = Path(args.output_dir) / "core_bfs_distances.csv"

    core_output = run_command(
        [
            args.binary,
            "bfs",
            "--graph",
            str(csr),
            "--source",
            str(source_id),
            "--threads",
            str(args.threads),
            "--output-csv",
            str(distances_csv),
        ]
    )

    distances = read_distance_csv(distances_csv)
    symbolic_path = bfs_path(graph, args.source, args.target)
    target_distance = distances.get(target_id, float("inf"))

    output_json = Path(args.output_dir) / "core_attack_path.json"
    output_json.parent.mkdir(parents=True, exist_ok=True)

    payload = {
        "source": args.source,
        "target": args.target,
        "source_id": source_id,
        "target_id": target_id,
        "algo": "core-bfs",
        "distance_from_core": target_distance,
        "symbolic_path": symbolic_path,
        "raw_output": core_output,
    }

    with output_json.open("w", encoding="utf-8") as f:
        json.dump(payload, f, indent=2)

    print("[GraphRush Security Core] BFS ejecutado en core C++.")
    print("[GraphRush Security Core] Camino simbólico: " + (" -> ".join(symbolic_path) if symbolic_path else "no encontrado"))
    print(f"[GraphRush Security Core] Distancia core hacia target: {target_distance}")
    print(f"[GraphRush Security Core] JSON: {output_json}")


def command_core_sssp(args: argparse.Namespace) -> None:
    graph, _, _, csr, node_to_id, id_to_node = ensure_core_artifacts(
        Path(args.graph),
        Path(args.output_dir),
        args.binary,
    )

    if args.source not in node_to_id:
        raise RuntimeError(f"No existe el nodo fuente: {args.source}")

    source_id = node_to_id[args.source]
    distances_csv = Path(args.output_dir) / "core_sssp_distances.csv"

    core_output = run_command(
        [
            args.binary,
            "sssp",
            "--graph",
            str(csr),
            "--source",
            str(source_id),
            "--algo",
            args.algo,
            "--delta",
            str(args.delta),
            "--threads",
            str(args.threads),
            "--compare",
            "--output-csv",
            str(distances_csv),
        ]
    )

    distances = read_distance_csv(distances_csv)
    symbolic_rows = [
        {"node_id": node_id, "node": id_to_node.get(node_id, str(node_id)), "distance": distance}
        for node_id, distance in sorted(distances.items())
    ]

    output_json = Path(args.output_dir) / "core_sssp_distances.json"
    with output_json.open("w", encoding="utf-8") as f:
        json.dump({"source": args.source, "source_id": source_id, "distances": symbolic_rows, "raw_output": core_output}, f, indent=2)

    print("[GraphRush Security Core] SSSP ejecutado en core C++.")
    print(f"[GraphRush Security Core] CSV: {distances_csv}")
    print(f"[GraphRush Security Core] JSON: {output_json}")


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="GraphRush Security Pack.")
    subparsers = parser.add_subparsers(dest="command", required=True)

    import_parser = subparsers.add_parser("import")
    import_parser.add_argument("--input", required=True)
    import_parser.add_argument("--output", required=True)
    import_parser.set_defaults(func=command_import)

    stats_parser = subparsers.add_parser("stats")
    stats_parser.add_argument("--graph", required=True)
    stats_parser.add_argument("--json", action="store_true")
    stats_parser.set_defaults(func=command_stats)

    rank_parser = subparsers.add_parser("rank")
    rank_parser.add_argument("--graph", required=True)
    rank_parser.add_argument("--method", default="pagerank")
    rank_parser.add_argument("--top", type=int, default=50)
    rank_parser.add_argument("--iterations", type=int, default=20)
    rank_parser.add_argument("--damping", type=float, default=0.85)
    rank_parser.add_argument("--output-csv")
    rank_parser.set_defaults(func=command_rank)

    path_parser = subparsers.add_parser("path")
    path_parser.add_argument("--graph", required=True)
    path_parser.add_argument("--source", required=True)
    path_parser.add_argument("--target", required=True)
    path_parser.add_argument("--algo", default="bfs")
    path_parser.add_argument("--output-json")
    path_parser.set_defaults(func=command_path)

    components_parser = subparsers.add_parser("components")
    components_parser.add_argument("--graph", required=True)
    components_parser.add_argument("--min-size", type=int, default=1)
    components_parser.add_argument("--output-json")
    components_parser.set_defaults(func=command_components)

    report_parser = subparsers.add_parser("report")
    report_parser.add_argument("--graph", required=True)
    report_parser.add_argument("--output", required=True)
    report_parser.add_argument("--top", type=int, default=20)
    report_parser.add_argument("--iterations", type=int, default=20)
    report_parser.add_argument("--source")
    report_parser.add_argument("--target")
    report_parser.set_defaults(func=command_report)

    export_parser = subparsers.add_parser("export-csr")
    export_parser.add_argument("--graph", required=True)
    export_parser.add_argument("--edge-list", required=True)
    export_parser.add_argument("--mapping", required=True)
    export_parser.add_argument("--output-csr", required=True)
    export_parser.add_argument("--binary", default="graphrush")
    export_parser.set_defaults(func=command_export_csr)

    core_rank_parser = subparsers.add_parser("core-rank")
    core_rank_parser.add_argument("--graph", required=True)
    core_rank_parser.add_argument("--output-dir", required=True)
    core_rank_parser.add_argument("--binary", default="graphrush")
    core_rank_parser.add_argument("--iterations", type=int, default=20)
    core_rank_parser.add_argument("--threads", type=int, default=4)
    core_rank_parser.add_argument("--top", type=int, default=20)
    core_rank_parser.set_defaults(func=command_core_rank)

    core_path_parser = subparsers.add_parser("core-path")
    core_path_parser.add_argument("--graph", required=True)
    core_path_parser.add_argument("--source", required=True)
    core_path_parser.add_argument("--target", required=True)
    core_path_parser.add_argument("--output-dir", required=True)
    core_path_parser.add_argument("--binary", default="graphrush")
    core_path_parser.add_argument("--threads", type=int, default=4)
    core_path_parser.set_defaults(func=command_core_path)

    core_sssp_parser = subparsers.add_parser("core-sssp")
    core_sssp_parser.add_argument("--graph", required=True)
    core_sssp_parser.add_argument("--source", required=True)
    core_sssp_parser.add_argument("--output-dir", required=True)
    core_sssp_parser.add_argument("--binary", default="graphrush")
    core_sssp_parser.add_argument("--algo", default="delta")
    core_sssp_parser.add_argument("--delta", type=float, default=2.0)
    core_sssp_parser.add_argument("--threads", type=int, default=4)
    core_sssp_parser.set_defaults(func=command_core_sssp)

    return parser


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    args.func(args)


if __name__ == "__main__":
    main()
