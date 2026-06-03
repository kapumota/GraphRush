#pragma once

#include <cstdint>
#include <memory>

#include "rust/cxx.h"
#include "graphrush/CsrGraph.hpp"
namespace graphrush {

std::unique_ptr<CsrGraph> load_graph(rust::Str path);

std::uint64_t node_count(const CsrGraph& graph);
std::uint64_t edge_count(const CsrGraph& graph);
std::uint64_t max_degree(const CsrGraph& graph);
double average_degree(const CsrGraph& graph);

} // namespace graphrush