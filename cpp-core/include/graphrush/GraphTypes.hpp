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

} // namespace graphrush
