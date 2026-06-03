#pragma once

#include <atomic>
#include <cstdint>
#include <limits>
#include <vector>

#include "graphrush/Algorithms.hpp"
#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"

namespace graphrush {

struct DeltaSteppingConfig {
    double delta = 1.0;
    std::uint32_t threads = 1;
};

class AtomicDistanceArray {
public:
    explicit AtomicDistanceArray(std::uint64_t size);

    bool relax(std::uint64_t node, double candidate);
    double get(std::uint64_t node) const;
    void set_source(std::uint64_t source);
    std::vector<double> to_vector() const;

private:
    static std::uint64_t encode(double value);
    static double decode(std::uint64_t value);

    std::vector<std::atomic<std::uint64_t>> distances_;
};

class BucketQueue {
public:
    explicit BucketQueue(double delta);

    std::uint64_t bucket_index(double distance) const;
    void add(std::uint64_t node, double distance);
    bool empty() const;
    std::uint64_t first_non_empty_bucket() const;
    std::vector<std::uint64_t> pop_bucket(std::uint64_t index);

private:
    double delta_;
    std::vector<std::vector<std::uint64_t>> buckets_;
};

class RelaxationEngine {
public:
    static std::vector<std::uint64_t> relax_light_edges(
        const CsrGraph& graph,
        const std::vector<std::uint64_t>& frontier,
        AtomicDistanceArray& distances,
        double delta,
        std::uint32_t threads
    );

    static std::vector<std::uint64_t> relax_heavy_edges(
        const CsrGraph& graph,
        const std::vector<std::uint64_t>& frontier,
        AtomicDistanceArray& distances,
        double delta,
        std::uint32_t threads
    );
};

class DeltaStepping {
public:
    static DijkstraOutput sssp(
        const CsrGraph& graph,
        std::uint64_t source,
        const DeltaSteppingConfig& config
    );
};

} // namespace graphrush
