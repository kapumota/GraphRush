#include "graphrush/ParallelAlgorithms.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

namespace graphrush {

namespace {

template <typename Function>
auto measure_ms(Function&& function) {
    const auto start = std::chrono::steady_clock::now();
    auto result = function();
    const auto end = std::chrono::steady_clock::now();
    result.summary.elapsed_ms =
        std::chrono::duration<double, std::milli>(end - start).count();
    return result;
}

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

bool atomic_min(std::atomic<std::uint64_t>& value, std::uint64_t candidate) {
    auto current = value.load(std::memory_order_relaxed);
    while (candidate < current) {
        if (value.compare_exchange_weak(
                current,
                candidate,
                std::memory_order_relaxed,
                std::memory_order_relaxed
            )) {
            return true;
        }
    }
    return false;
}

} // namespace

BfsOutput ParallelAlgorithms::bfs_frontier(
    const CsrGraph& graph,
    std::uint64_t source,
    std::uint32_t threads
) {
    ensure_source(graph, source);
    configure_threads(threads);

    return measure_ms([&]() {
        const auto nodes = graph.node_count();
        const auto& offsets = graph.offsets();
        const auto& neighbors = graph.neighbors();

        std::vector<std::atomic<std::int64_t>> atomic_distances(nodes);
        for (auto& distance : atomic_distances) {
            distance.store(-1, std::memory_order_relaxed);
        }

        atomic_distances[source].store(0, std::memory_order_relaxed);

        std::vector<std::uint64_t> frontier;
        frontier.push_back(source);

        std::int64_t depth = 0;
        std::uint64_t reached = 1;

        while (!frontier.empty()) {
            std::vector<std::uint64_t> next_frontier;

#pragma omp parallel
            {
                std::vector<std::uint64_t> local_next;

#pragma omp for schedule(dynamic, 64)
                for (std::int64_t i = 0; i < static_cast<std::int64_t>(frontier.size()); ++i) {
                    const auto current = frontier[static_cast<std::size_t>(i)];

                    for (std::uint64_t idx = offsets[current]; idx < offsets[current + 1]; ++idx) {
                        const auto neighbor = neighbors[idx];
                        std::int64_t expected = -1;

                        if (atomic_distances[neighbor].compare_exchange_strong(
                                expected,
                                depth + 1,
                                std::memory_order_relaxed,
                                std::memory_order_relaxed
                            )) {
                            local_next.push_back(neighbor);
                        }
                    }
                }

#pragma omp critical
                {
                    next_frontier.insert(
                        next_frontier.end(),
                        local_next.begin(),
                        local_next.end()
                    );
                }
            }

            reached += static_cast<std::uint64_t>(next_frontier.size());
            frontier.swap(next_frontier);
            ++depth;
        }

        BfsOutput output;
        output.distances.assign(nodes, -1);

        std::uint64_t max_depth = 0;
        for (std::uint64_t node = 0; node < nodes; ++node) {
            const auto value = atomic_distances[node].load(std::memory_order_relaxed);
            output.distances[node] = value;
            if (value >= 0) {
                max_depth = std::max(max_depth, static_cast<std::uint64_t>(value));
            }
        }

        output.summary.source = source;
        output.summary.reached_nodes = reached;
        output.summary.max_depth = max_depth;
        return output;
    });
}

ComponentsOutput ParallelAlgorithms::connected_components_label_propagation(
    const CsrGraph& graph,
    std::uint32_t threads
) {
    configure_threads(threads);

    return measure_ms([&]() {
        const auto nodes = graph.node_count();
        const auto& offsets = graph.offsets();
        const auto& neighbors = graph.neighbors();

        std::vector<std::atomic<std::uint64_t>> labels(nodes);
        for (std::uint64_t node = 0; node < nodes; ++node) {
            labels[node].store(node, std::memory_order_relaxed);
        }

        bool changed = true;
        std::uint64_t iterations = 0;
        const std::uint64_t max_iterations = std::max<std::uint64_t>(1, nodes);

        while (changed && iterations < max_iterations) {
            changed = false;

#pragma omp parallel for schedule(dynamic, 1024)
            for (std::int64_t u_signed = 0; u_signed < static_cast<std::int64_t>(nodes); ++u_signed) {
                const auto u = static_cast<std::uint64_t>(u_signed);
                const auto label_u = labels[u].load(std::memory_order_relaxed);

                for (std::uint64_t idx = offsets[u]; idx < offsets[u + 1]; ++idx) {
                    const auto v = neighbors[idx];
                    const auto label_v = labels[v].load(std::memory_order_relaxed);
                    const auto best = std::min(label_u, label_v);

                    if (atomic_min(labels[u], best) || atomic_min(labels[v], best)) {
                        changed = true;
                    }
                }
            }

            ++iterations;
        }

        ComponentsOutput output;
        output.labels.assign(nodes, 0);

        std::unordered_map<std::uint64_t, std::uint64_t> counts;
        for (std::uint64_t node = 0; node < nodes; ++node) {
            const auto label = labels[node].load(std::memory_order_relaxed);
            output.labels[node] = label;
            counts[label] += 1;
        }

        std::uint64_t largest = 0;
        for (const auto& [_, size] : counts) {
            largest = std::max(largest, size);
        }

        output.summary.components = static_cast<std::uint64_t>(counts.size());
        output.summary.largest_component = largest;
        return output;
    });
}

PagerankOutput ParallelAlgorithms::pagerank_pull_atomic(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping,
    std::uint32_t threads
) {
    if (damping <= 0.0 || damping >= 1.0) {
        throw std::runtime_error("El factor damping debe estar entre 0 y 1.");
    }

    configure_threads(threads);

    return measure_ms([&]() {
        const auto nodes = graph.node_count();
        const auto& offsets = graph.offsets();
        const auto& neighbors = graph.neighbors();

        PagerankOutput output;
        output.summary.iterations = iterations;
        output.summary.damping = damping;

        if (nodes == 0) {
            output.summary.l1_delta = 0.0;
            return output;
        }

        output.scores.assign(nodes, 1.0 / static_cast<double>(nodes));
        std::vector<double> next(nodes, 0.0);

        double last_delta = 0.0;

        for (std::uint32_t iteration = 0; iteration < iterations; ++iteration) {
            const double base = (1.0 - damping) / static_cast<double>(nodes);
            std::fill(next.begin(), next.end(), base);

            double dangling_mass = 0.0;

#pragma omp parallel for schedule(dynamic, 1024) reduction(+:dangling_mass)
            for (std::int64_t node_signed = 0; node_signed < static_cast<std::int64_t>(nodes); ++node_signed) {
                const auto node = static_cast<std::uint64_t>(node_signed);
                const auto degree = offsets[node + 1] - offsets[node];

                if (degree == 0) {
                    dangling_mass += output.scores[node];
                    continue;
                }

                const double contribution =
                    damping * output.scores[node] / static_cast<double>(degree);

                for (std::uint64_t idx = offsets[node]; idx < offsets[node + 1]; ++idx) {
                    const auto neighbor = neighbors[idx];
#pragma omp atomic
                    next[neighbor] += contribution;
                }
            }

            const double dangling_contribution =
                damping * dangling_mass / static_cast<double>(nodes);

#pragma omp parallel for schedule(static)
            for (std::int64_t node_signed = 0; node_signed < static_cast<std::int64_t>(nodes); ++node_signed) {
                next[static_cast<std::size_t>(node_signed)] += dangling_contribution;
            }

            last_delta = 0.0;

#pragma omp parallel for schedule(static) reduction(+:last_delta)
            for (std::int64_t node_signed = 0; node_signed < static_cast<std::int64_t>(nodes); ++node_signed) {
                const auto node = static_cast<std::size_t>(node_signed);
                last_delta += std::abs(next[node] - output.scores[node]);
            }

            output.scores.swap(next);
        }

        output.summary.l1_delta = last_delta;
        return output;
    });
}

} // namespace graphrush
