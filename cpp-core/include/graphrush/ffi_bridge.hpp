#pragma once

#include <cstdint>
#include <memory>

#include "rust/cxx.h"
#include "graphrush/CsrGraph.hpp"

namespace graphrush {

struct GraphStats;

std::unique_ptr<CsrGraph> load_graph(rust::Str path);

GraphStats import_graph(
    rust::Str input_path,
    rust::Str output_path,
    rust::Str format,
    bool directed,
    bool deduplicate
);

GraphStats graph_stats(const CsrGraph& graph);

std::uint64_t node_count(const CsrGraph& graph);
std::uint64_t edge_count(const CsrGraph& graph);
std::uint64_t max_degree(const CsrGraph& graph);
double average_degree(const CsrGraph& graph);
std::uint64_t memory_bytes(const CsrGraph& graph);
bool validate_graph(const CsrGraph& graph);

rust::String run_bfs_report(const CsrGraph& graph, std::uint64_t source);
rust::String run_components_report(const CsrGraph& graph);
rust::String run_pagerank_report(const CsrGraph& graph, std::uint32_t iterations, double damping);
rust::String run_dijkstra_report(const CsrGraph& graph, std::uint64_t source);

rust::String run_bfs_json_report(const CsrGraph& graph, std::uint64_t source);
rust::String run_components_json_report(const CsrGraph& graph);
rust::String run_pagerank_top_report(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping,
    std::uint64_t top_k
);
rust::String run_pagerank_json_report(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping,
    std::uint64_t top_k
);
rust::String run_dijkstra_json_report(const CsrGraph& graph, std::uint64_t source);

void write_bfs_distances_csv(const CsrGraph& graph, std::uint64_t source, rust::Str output_path);
void write_dijkstra_distances_csv(const CsrGraph& graph, std::uint64_t source, rust::Str output_path);

rust::String run_parallel_bfs_report(
    const CsrGraph& graph,
    std::uint64_t source,
    std::uint32_t threads
);
rust::String run_parallel_components_report(
    const CsrGraph& graph,
    std::uint32_t threads
);
rust::String run_parallel_pagerank_report(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping,
    std::uint32_t threads,
    std::uint64_t top_k
);

} // namespace graphrush
