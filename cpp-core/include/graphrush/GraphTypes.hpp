#pragma once

#include <cstdint>

namespace graphrush {

enum class GraphTextFormat : std::uint32_t {
    EdgeList = 0,
    Snap = 1,
    Csv = 2
};

struct GraphBuildOptions {
    GraphTextFormat format = GraphTextFormat::EdgeList;
    bool directed = true;
    bool deduplicate = true;
};

struct CoreGraphStats {
    std::uint64_t nodes = 0;
    std::uint64_t edges = 0;
    std::uint64_t max_degree = 0;
    double average_degree = 0.0;
    std::uint64_t memory_bytes = 0;
    double load_time_ms = 0.0;
    bool valid = false;
};

struct BfsResult {
    std::uint64_t source = 0;
    std::uint64_t reached_nodes = 0;
    std::uint64_t max_depth = 0;
    double elapsed_ms = 0.0;
};

struct ComponentsResult {
    std::uint64_t components = 0;
    std::uint64_t largest_component = 0;
    double elapsed_ms = 0.0;
};

struct PagerankResult {
    std::uint32_t iterations = 0;
    double damping = 0.85;
    double l1_delta = 0.0;
    double elapsed_ms = 0.0;
};

struct DijkstraResult {
    std::uint64_t source = 0;
    std::uint64_t reached_nodes = 0;
    double max_distance = 0.0;
    double elapsed_ms = 0.0;
};

} // namespace graphrush
