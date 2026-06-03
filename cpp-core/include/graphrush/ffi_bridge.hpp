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

} // namespace graphrush