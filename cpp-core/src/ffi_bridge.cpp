#include "graphrush/ffi_bridge.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include "graphrush/CsrGraph.hpp"
#include "graphrush/Algorithms.hpp"
#include "graphrush-cli/src/ffi.rs.h"

namespace graphrush {

namespace {

GraphTextFormat parse_format(rust::Str format) {
    const std::string value(format.data(), format.size());

    if (value == "edgelist") {
        return GraphTextFormat::EdgeList;
    }

    if (value == "snap") {
        return GraphTextFormat::Snap;
    }

    if (value == "csv") {
        return GraphTextFormat::Csv;
    }

    throw std::runtime_error("Formato de grafo no soportado: " + value);
}


GraphStats to_ffi_stats(const CoreGraphStats& core_stats) {
    GraphStats stats;
    stats.nodes = core_stats.nodes;
    stats.edges = core_stats.edges;
    stats.max_degree = core_stats.max_degree;
    stats.average_degree = core_stats.average_degree;
    stats.memory_bytes = core_stats.memory_bytes;
    stats.load_time_ms = core_stats.load_time_ms;
    stats.valid = core_stats.valid;
    return stats;
}

} // namespace

std::unique_ptr<CsrGraph> load_graph(rust::Str path) {
    const std::string path_string(path.data(), path.size());
    return CsrGraph::load_binary(path_string);
}

GraphStats import_graph(
    rust::Str input_path,
    rust::Str output_path,
    rust::Str format,
    bool directed,
    bool deduplicate
) {
    const std::string input_string(input_path.data(), input_path.size());
    const std::string output_string(output_path.data(), output_path.size());

    GraphBuildOptions options;
    options.format = parse_format(format);
    options.directed = directed;
    options.deduplicate = deduplicate;

    const auto start = std::chrono::steady_clock::now();
    auto graph = CsrGraph::load_from_text(input_string, options);
    const auto end = std::chrono::steady_clock::now();

    graph->write_binary(output_string);

    const auto elapsed = std::chrono::duration<double, std::milli>(end - start).count();
    return to_ffi_stats(graph->stats(elapsed));
}

GraphStats graph_stats(const CsrGraph& graph) {
    return to_ffi_stats(graph.stats(0.0));
}

std::uint64_t node_count(const CsrGraph& graph) {
    return graph.node_count();
}

std::uint64_t edge_count(const CsrGraph& graph) {
    return graph.edge_count();
}

std::uint64_t max_degree(const CsrGraph& graph) {
    return graph.max_degree();
}

double average_degree(const CsrGraph& graph) {
    return graph.average_degree();
}

std::uint64_t memory_bytes(const CsrGraph& graph) {
    return graph.memory_bytes();
}

bool validate_graph(const CsrGraph& graph) {
    return graph.validate();
}


namespace {

rust::String make_rust_string(const std::string& value) {
    return rust::String(value);
}

} // namespace

rust::String run_bfs_report(const CsrGraph& graph, std::uint64_t source) {
    const auto result = SequentialAlgorithms::bfs(graph, source);
    std::string output;
    output += "[GraphRush] BFS secuencial completado.\n";
    output += "[GraphRush] Fuente: " + std::to_string(result.summary.source) + "\n";
    output += "[GraphRush] Nodos alcanzados: " + std::to_string(result.summary.reached_nodes) + "\n";
    output += "[GraphRush] Profundidad máxima: " + std::to_string(result.summary.max_depth) + "\n";
    output += "[GraphRush] Tiempo: " + std::to_string(result.summary.elapsed_ms) + " ms\n";
    return make_rust_string(output);
}

rust::String run_components_report(const CsrGraph& graph) {
    const auto result = SequentialAlgorithms::connected_components(graph);
    std::string output;
    output += "[GraphRush] Componentes secuenciales completados.\n";
    output += "[GraphRush] Componentes: " + std::to_string(result.summary.components) + "\n";
    output += "[GraphRush] Componente mayor: " + std::to_string(result.summary.largest_component) + "\n";
    output += "[GraphRush] Tiempo: " + std::to_string(result.summary.elapsed_ms) + " ms\n";
    return make_rust_string(output);
}

rust::String run_pagerank_report(const CsrGraph& graph, std::uint32_t iterations, double damping) {
    const auto result = SequentialAlgorithms::pagerank(graph, iterations, damping);
    std::string output;
    output += "[GraphRush] PageRank secuencial completado.\n";
    output += "[GraphRush] Iteraciones: " + std::to_string(result.summary.iterations) + "\n";
    output += "[GraphRush] Damping: " + std::to_string(result.summary.damping) + "\n";
    output += "[GraphRush] Delta L1 final: " + std::to_string(result.summary.l1_delta) + "\n";
    output += "[GraphRush] Tiempo: " + std::to_string(result.summary.elapsed_ms) + " ms\n";
    return make_rust_string(output);
}

rust::String run_dijkstra_report(const CsrGraph& graph, std::uint64_t source) {
    const auto result = SequentialAlgorithms::dijkstra(graph, source);
    std::string output;
    output += "[GraphRush] Dijkstra secuencial completado.\n";
    output += "[GraphRush] Fuente: " + std::to_string(result.summary.source) + "\n";
    output += "[GraphRush] Nodos alcanzados: " + std::to_string(result.summary.reached_nodes) + "\n";
    output += "[GraphRush] Distancia máxima: " + std::to_string(result.summary.max_distance) + "\n";
    output += "[GraphRush] Tiempo: " + std::to_string(result.summary.elapsed_ms) + " ms\n";
    return make_rust_string(output);
}


std::string json_bool(bool value) {
    return value ? "true" : "false";
}

rust::String run_bfs_json_report(const CsrGraph& graph, std::uint64_t source) {
    const auto result = SequentialAlgorithms::bfs(graph, source);
    std::ostringstream output;
    output << "{\n";
    output << "  \"algorithm\": \"bfs\",\n";
    output << "  \"source\": " << result.summary.source << ",\n";
    output << "  \"reached_nodes\": " << result.summary.reached_nodes << ",\n";
    output << "  \"max_depth\": " << result.summary.max_depth << ",\n";
    output << "  \"elapsed_ms\": " << std::fixed << std::setprecision(6) << result.summary.elapsed_ms << "\n";
    output << "}\n";
    return make_rust_string(output.str());
}

rust::String run_components_json_report(const CsrGraph& graph) {
    const auto result = SequentialAlgorithms::connected_components(graph);
    std::ostringstream output;
    output << "{\n";
    output << "  \"algorithm\": \"components\",\n";
    output << "  \"components\": " << result.summary.components << ",\n";
    output << "  \"largest_component\": " << result.summary.largest_component << ",\n";
    output << "  \"elapsed_ms\": " << std::fixed << std::setprecision(6) << result.summary.elapsed_ms << "\n";
    output << "}\n";
    return make_rust_string(output.str());
}

rust::String run_pagerank_top_report(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping,
    std::uint64_t top_k
) {
    const auto result = SequentialAlgorithms::pagerank(graph, iterations, damping);

    std::vector<std::pair<double, std::uint64_t>> ranked;
    ranked.reserve(result.scores.size());

    for (std::uint64_t node = 0; node < result.scores.size(); ++node) {
        ranked.emplace_back(result.scores[node], node);
    }

    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
        if (a.first == b.first) {
            return a.second < b.second;
        }
        return a.first > b.first;
    });

    const std::uint64_t limit = std::min<std::uint64_t>(top_k, ranked.size());

    std::ostringstream output;
    output << "[GraphRush] PageRank secuencial completado.\n";
    output << "[GraphRush] Iteraciones: " << result.summary.iterations << "\n";
    output << "[GraphRush] Damping: " << result.summary.damping << "\n";
    output << "[GraphRush] Delta L1 final: " << result.summary.l1_delta << "\n";
    output << "[GraphRush] Top-k: " << limit << "\n";

    for (std::uint64_t i = 0; i < limit; ++i) {
        output << "[GraphRush] rank=" << (i + 1)
               << " node=" << ranked[i].second
               << " score=" << std::fixed << std::setprecision(10) << ranked[i].first
               << "\n";
    }

    output << "[GraphRush] Tiempo: " << result.summary.elapsed_ms << " ms\n";
    return make_rust_string(output.str());
}

rust::String run_pagerank_json_report(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping,
    std::uint64_t top_k
) {
    const auto result = SequentialAlgorithms::pagerank(graph, iterations, damping);

    std::vector<std::pair<double, std::uint64_t>> ranked;
    ranked.reserve(result.scores.size());

    for (std::uint64_t node = 0; node < result.scores.size(); ++node) {
        ranked.emplace_back(result.scores[node], node);
    }

    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
        if (a.first == b.first) {
            return a.second < b.second;
        }
        return a.first > b.first;
    });

    const std::uint64_t limit = std::min<std::uint64_t>(top_k, ranked.size());

    std::ostringstream output;
    output << "{\n";
    output << "  \"algorithm\": \"pagerank\",\n";
    output << "  \"iterations\": " << result.summary.iterations << ",\n";
    output << "  \"damping\": " << result.summary.damping << ",\n";
    output << "  \"l1_delta\": " << std::fixed << std::setprecision(10) << result.summary.l1_delta << ",\n";
    output << "  \"elapsed_ms\": " << std::fixed << std::setprecision(6) << result.summary.elapsed_ms << ",\n";
    output << "  \"top\": [\n";

    for (std::uint64_t i = 0; i < limit; ++i) {
        output << "    {\"rank\": " << (i + 1)
               << ", \"node\": " << ranked[i].second
               << ", \"score\": " << std::fixed << std::setprecision(10) << ranked[i].first
               << "}";
        if (i + 1 < limit) {
            output << ",";
        }
        output << "\n";
    }

    output << "  ]\n";
    output << "}\n";
    return make_rust_string(output.str());
}

rust::String run_dijkstra_json_report(const CsrGraph& graph, std::uint64_t source) {
    const auto result = SequentialAlgorithms::dijkstra(graph, source);
    std::ostringstream output;
    output << "{\n";
    output << "  \"algorithm\": \"dijkstra\",\n";
    output << "  \"source\": " << result.summary.source << ",\n";
    output << "  \"reached_nodes\": " << result.summary.reached_nodes << ",\n";
    output << "  \"max_distance\": " << std::fixed << std::setprecision(6) << result.summary.max_distance << ",\n";
    output << "  \"elapsed_ms\": " << std::fixed << std::setprecision(6) << result.summary.elapsed_ms << "\n";
    output << "}\n";
    return make_rust_string(output.str());
}

void write_bfs_distances_csv(
    const CsrGraph& graph,
    std::uint64_t source,
    rust::Str output_path
) {
    const auto result = SequentialAlgorithms::bfs(graph, source);
    const std::string path(output_path.data(), output_path.size());

    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("No se pudo escribir el CSV de distancias BFS: " + path);
    }

    output << "node,distance\n";
    for (std::uint64_t node = 0; node < result.distances.size(); ++node) {
        output << node << "," << result.distances[node] << "\n";
    }
}

void write_dijkstra_distances_csv(
    const CsrGraph& graph,
    std::uint64_t source,
    rust::Str output_path
) {
    const auto result = SequentialAlgorithms::dijkstra(graph, source);
    const std::string path(output_path.data(), output_path.size());

    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("No se pudo escribir el CSV de distancias Dijkstra: " + path);
    }

    output << "node,distance\n";
    for (std::uint64_t node = 0; node < result.distances.size(); ++node) {
        if (std::isfinite(result.distances[node])) {
            output << node << "," << result.distances[node] << "\n";
        } else {
            output << node << ",inf\n";
        }
    }
}

} // namespace graphrush
