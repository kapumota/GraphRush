#include "graphrush/ffi_bridge.hpp"

#include <chrono>
#include <stdexcept>
#include <string>

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

    const auto elapsed =
        std::chrono::duration<double, std::milli>(end - start).count();

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

} // namespace graphrush
