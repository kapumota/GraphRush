#pragma once

#include <cstdint>
#include <vector>

#include "graphrush/Algorithms.hpp"
#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"

namespace graphrush {

class ParallelAlgorithms {
public:
    static BfsOutput bfs_frontier(
        const CsrGraph& graph,
        std::uint64_t source,
        std::uint32_t threads
    );

    static ComponentsOutput connected_components_label_propagation(
        const CsrGraph& graph,
        std::uint32_t threads
    );

    static PagerankOutput pagerank_pull_atomic(
        const CsrGraph& graph,
        std::uint32_t iterations,
        double damping,
        std::uint32_t threads
    );
};

} // namespace graphrush
