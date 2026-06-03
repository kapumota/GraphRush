#include "graphrush/ffi_bridge.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include "graphrush/CsrGraph.hpp"

namespace graphrush {

std::unique_ptr<CsrGraph> load_graph(rust::Str path) {
    const std::string path_string(path.data(), path.size());
    return CsrGraph::load_from_edge_list(path_string);
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

} // namespace graphrush