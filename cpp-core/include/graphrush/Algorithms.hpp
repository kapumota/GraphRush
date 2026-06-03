#pragma once

#include <cstdint>
#include <vector>

#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"

namespace graphrush {

struct BfsOutput {
    BfsResult summary;
    std::vector<std::int64_t> distances;
};

struct ComponentsOutput {
    ComponentsResult summary;
    std::vector<std::uint64_t> labels;
};

struct PagerankOutput {
    PagerankResult summary;
    std::vector<double> scores;
};

struct DijkstraOutput {
    DijkstraResult summary;
    std::vector<double> distances;
};

class SequentialAlgorithms {
public:
    static BfsOutput bfs(const CsrGraph& graph, std::uint64_t source);

    static ComponentsOutput connected_components(const CsrGraph& graph);

    static PagerankOutput pagerank(
        const CsrGraph& graph,
        std::uint32_t iterations,
        double damping
    );

    static DijkstraOutput dijkstra(const CsrGraph& graph, std::uint64_t source);
};

} // namespace graphrush
