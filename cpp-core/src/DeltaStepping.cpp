#include "graphrush/DeltaStepping.hpp"

#include <algorithm>
#include <cmath>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_set>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace graphrush {

namespace {

constexpr std::uint64_t INF_BITS = std::numeric_limits<std::uint64_t>::max();

void configure_threads(std::uint32_t threads) {
#ifdef _OPENMP
    if (threads > 0) {
        omp_set_num_threads(static_cast<int>(threads));
    }
#else
    (void)threads;
#endif
}

void ensure_source(const CsrGraph& graph, std::uint64_t source) {
    if (source >= graph.node_count()) {
        throw std::runtime_error("El nodo fuente está fuera del rango del grafo.");
    }
}

} // namespace

AtomicDistanceArray::AtomicDistanceArray(std::uint64_t size)
    : distances_(size) {
    for (auto& value : distances_) {
        value.store(INF_BITS, std::memory_order_relaxed);
    }
}

std::uint64_t AtomicDistanceArray::encode(double value) {
    if (!std::isfinite(value)) {
        return INF_BITS;
    }

    // GraphRush Fase 5 usa peso unitario. Escalamos para conservar la API double.
    return static_cast<std::uint64_t>(std::llround(value * 1'000'000.0));
}

double AtomicDistanceArray::decode(std::uint64_t value) {
    if (value == INF_BITS) {
        return std::numeric_limits<double>::infinity();
    }

    return static_cast<double>(value) / 1'000'000.0;
}

bool AtomicDistanceArray::relax(std::uint64_t node, double candidate) {
    const auto encoded_candidate = encode(candidate);
    auto current = distances_[node].load(std::memory_order_relaxed);

    while (encoded_candidate < current) {
        if (distances_[node].compare_exchange_weak(
                current,
                encoded_candidate,
                std::memory_order_relaxed,
                std::memory_order_relaxed
            )) {
            return true;
        }
    }

    return false;
}

double AtomicDistanceArray::get(std::uint64_t node) const {
    return decode(distances_[node].load(std::memory_order_relaxed));
}

void AtomicDistanceArray::set_source(std::uint64_t source) {
    distances_[source].store(encode(0.0), std::memory_order_relaxed);
}

std::vector<double> AtomicDistanceArray::to_vector() const {
    std::vector<double> result(distances_.size());

    for (std::uint64_t node = 0; node < distances_.size(); ++node) {
        result[node] = get(node);
    }

    return result;
}

BucketQueue::BucketQueue(double delta)
    : delta_(delta) {
    if (delta_ <= 0.0) {
        throw std::runtime_error("Delta debe ser positivo.");
    }
}

std::uint64_t BucketQueue::bucket_index(double distance) const {
    return static_cast<std::uint64_t>(std::floor(distance / delta_));
}

void BucketQueue::add(std::uint64_t node, double distance) {
    const auto index = bucket_index(distance);

    if (index >= buckets_.size()) {
        buckets_.resize(index + 1);
    }

    buckets_[index].push_back(node);
}

bool BucketQueue::empty() const {
    for (const auto& bucket : buckets_) {
        if (!bucket.empty()) {
            return false;
        }
    }
    return true;
}

std::uint64_t BucketQueue::first_non_empty_bucket() const {
    for (std::uint64_t index = 0; index < buckets_.size(); ++index) {
        if (!buckets_[index].empty()) {
            return index;
        }
    }

    return buckets_.size();
}

std::vector<std::uint64_t> BucketQueue::pop_bucket(std::uint64_t index) {
    if (index >= buckets_.size()) {
        return {};
    }

    auto result = std::move(buckets_[index]);
    buckets_[index].clear();
    return result;
}

std::vector<std::uint64_t> RelaxationEngine::relax_light_edges(
    const CsrGraph& graph,
    const std::vector<std::uint64_t>& frontier,
    AtomicDistanceArray& distances,
    double delta,
    std::uint32_t threads
) {
    configure_threads(threads);

    const auto& offsets = graph.offsets();
    const auto& neighbors = graph.neighbors();

    std::vector<std::uint64_t> relaxed;

#pragma omp parallel
    {
        std::vector<std::uint64_t> local_relaxed;

#pragma omp for schedule(dynamic, 64)
        for (std::int64_t i = 0; i < static_cast<std::int64_t>(frontier.size()); ++i) {
            const auto current = frontier[static_cast<std::size_t>(i)];
            const auto base_distance = distances.get(current);

            for (std::uint64_t idx = offsets[current]; idx < offsets[current + 1]; ++idx) {
                const auto neighbor = neighbors[idx];

                const auto weight = graph.edge_weight(idx);
                if (weight <= delta) {
                    const auto candidate = base_distance + weight;
                    if (distances.relax(neighbor, candidate)) {
                        local_relaxed.push_back(neighbor);
                    }
                }
            }
        }

#pragma omp critical
        {
            relaxed.insert(relaxed.end(), local_relaxed.begin(), local_relaxed.end());
        }
    }

    return relaxed;
}

std::vector<std::uint64_t> RelaxationEngine::relax_heavy_edges(
    const CsrGraph& graph,
    const std::vector<std::uint64_t>& frontier,
    AtomicDistanceArray& distances,
    double delta,
    std::uint32_t threads
) {
    configure_threads(threads);

    const auto& offsets = graph.offsets();
    const auto& neighbors = graph.neighbors();

    std::vector<std::uint64_t> relaxed;

#pragma omp parallel
    {
        std::vector<std::uint64_t> local_relaxed;

#pragma omp for schedule(dynamic, 64)
        for (std::int64_t i = 0; i < static_cast<std::int64_t>(frontier.size()); ++i) {
            const auto current = frontier[static_cast<std::size_t>(i)];
            const auto base_distance = distances.get(current);

            for (std::uint64_t idx = offsets[current]; idx < offsets[current + 1]; ++idx) {
                const auto neighbor = neighbors[idx];

                const auto weight = graph.edge_weight(idx);
                if (weight > delta) {
                    const auto candidate = base_distance + weight;
                    if (distances.relax(neighbor, candidate)) {
                        local_relaxed.push_back(neighbor);
                    }
                }
            }
        }

#pragma omp critical
        {
            relaxed.insert(relaxed.end(), local_relaxed.begin(), local_relaxed.end());
        }
    }

    return relaxed;
}

DijkstraOutput DeltaStepping::sssp(
    const CsrGraph& graph,
    std::uint64_t source,
    const DeltaSteppingConfig& config
) {
    ensure_source(graph, source);

    if (config.delta <= 0.0) {
        throw std::runtime_error("Delta debe ser positivo.");
    }

    configure_threads(config.threads);

    const auto start = std::chrono::steady_clock::now();

    AtomicDistanceArray distances(graph.node_count());
    distances.set_source(source);

    BucketQueue buckets(config.delta);
    buckets.add(source, 0.0);

    while (!buckets.empty()) {
        const auto bucket_index = buckets.first_non_empty_bucket();
        if (bucket_index == graph.node_count() + graph.edge_count() + 1) {
            break;
        }

        std::vector<std::uint64_t> request = buckets.pop_bucket(bucket_index);
        std::vector<std::uint64_t> settled;

        while (!request.empty()) {
            settled.insert(settled.end(), request.begin(), request.end());

            const auto relaxed_light = RelaxationEngine::relax_light_edges(
                graph,
                request,
                distances,
                config.delta,
                config.threads
            );

            request.clear();
            for (const auto node : relaxed_light) {
                const auto new_bucket = buckets.bucket_index(distances.get(node));
                if (new_bucket == bucket_index) {
                    request.push_back(node);
                } else {
                    buckets.add(node, distances.get(node));
                }
            }
        }

        const auto relaxed_heavy = RelaxationEngine::relax_heavy_edges(
            graph,
            settled,
            distances,
            config.delta,
            config.threads
        );

        for (const auto node : relaxed_heavy) {
            buckets.add(node, distances.get(node));
        }
    }

    auto output_distances = distances.to_vector();

    std::uint64_t reached = 0;
    double max_distance = 0.0;
    for (const auto distance : output_distances) {
        if (std::isfinite(distance)) {
            ++reached;
            max_distance = std::max(max_distance, distance);
        }
    }

    const auto end = std::chrono::steady_clock::now();

    DijkstraOutput output;
    output.distances = std::move(output_distances);
    output.summary.source = source;
    output.summary.reached_nodes = reached;
    output.summary.max_distance = max_distance;
    output.summary.elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();

    return output;
}

} // namespace graphrush
