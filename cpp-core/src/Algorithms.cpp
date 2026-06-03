#include "graphrush/Algorithms.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

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

void ensure_source(const CsrGraph& graph, std::uint64_t source) {
    if (source >= graph.node_count()) {
        throw std::runtime_error("El nodo fuente está fuera del rango del grafo.");
    }
}

} // namespace

BfsOutput SequentialAlgorithms::bfs(const CsrGraph& graph, std::uint64_t source) {
    ensure_source(graph, source);

    return measure_ms([&]() {
        const auto nodes = graph.node_count();
        const auto& offsets = graph.offsets();
        const auto& neighbors = graph.neighbors();

        BfsOutput output;
        output.distances.assign(nodes, -1);

        std::queue<std::uint64_t> queue;
        output.distances[source] = 0;
        queue.push(source);

        std::uint64_t reached = 1;
        std::uint64_t max_depth = 0;

        while (!queue.empty()) {
            const auto current = queue.front();
            queue.pop();

            const auto base_distance = output.distances[current];

            for (std::uint64_t idx = offsets[current]; idx < offsets[current + 1]; ++idx) {
                const auto neighbor = neighbors[idx];

                if (output.distances[neighbor] == -1) {
                    output.distances[neighbor] = base_distance + 1;
                    max_depth = std::max<std::uint64_t>(
                        max_depth,
                        static_cast<std::uint64_t>(output.distances[neighbor])
                    );
                    ++reached;
                    queue.push(neighbor);
                }
            }
        }

        output.summary.source = source;
        output.summary.reached_nodes = reached;
        output.summary.max_depth = max_depth;
        return output;
    });
}

ComponentsOutput SequentialAlgorithms::connected_components(const CsrGraph& graph) {
    return measure_ms([&]() {
        const auto nodes = graph.node_count();
        const auto& offsets = graph.offsets();
        const auto& neighbors = graph.neighbors();

        ComponentsOutput output;
        output.labels.assign(nodes, std::numeric_limits<std::uint64_t>::max());

        std::uint64_t components = 0;
        std::uint64_t largest = 0;

        std::queue<std::uint64_t> queue;

        for (std::uint64_t source = 0; source < nodes; ++source) {
            if (output.labels[source] != std::numeric_limits<std::uint64_t>::max()) {
                continue;
            }

            const std::uint64_t label = components;
            std::uint64_t size = 1;

            output.labels[source] = label;
            queue.push(source);

            while (!queue.empty()) {
                const auto current = queue.front();
                queue.pop();

                for (std::uint64_t idx = offsets[current]; idx < offsets[current + 1]; ++idx) {
                    const auto neighbor = neighbors[idx];

                    if (output.labels[neighbor] == std::numeric_limits<std::uint64_t>::max()) {
                        output.labels[neighbor] = label;
                        ++size;
                        queue.push(neighbor);
                    }
                }
            }

            largest = std::max(largest, size);
            ++components;
        }

        output.summary.components = components;
        output.summary.largest_component = largest;
        return output;
    });
}

PagerankOutput SequentialAlgorithms::pagerank(
    const CsrGraph& graph,
    std::uint32_t iterations,
    double damping
) {
    if (damping <= 0.0 || damping >= 1.0) {
        throw std::runtime_error("El factor damping debe estar entre 0 y 1.");
    }

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

            for (std::uint64_t node = 0; node < nodes; ++node) {
                const auto degree = offsets[node + 1] - offsets[node];

                if (degree == 0) {
                    dangling_mass += output.scores[node];
                    continue;
                }

                const double contribution =
                    damping * output.scores[node] / static_cast<double>(degree);

                for (std::uint64_t idx = offsets[node]; idx < offsets[node + 1]; ++idx) {
                    next[neighbors[idx]] += contribution;
                }
            }

            const double dangling_contribution =
                damping * dangling_mass / static_cast<double>(nodes);

            for (auto& value : next) {
                value += dangling_contribution;
            }

            last_delta = 0.0;
            for (std::uint64_t node = 0; node < nodes; ++node) {
                last_delta += std::abs(next[node] - output.scores[node]);
            }

            output.scores.swap(next);
        }

        output.summary.l1_delta = last_delta;
        return output;
    });
}

DijkstraOutput SequentialAlgorithms::dijkstra(const CsrGraph& graph, std::uint64_t source) {
    ensure_source(graph, source);

    return measure_ms([&]() {
        const auto nodes = graph.node_count();
        const auto& offsets = graph.offsets();
        const auto& neighbors = graph.neighbors();

        DijkstraOutput output;
        output.distances.assign(nodes, std::numeric_limits<double>::infinity());

        using QueueEntry = std::pair<double, std::uint64_t>;
        std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> queue;

        output.distances[source] = 0.0;
        queue.emplace(0.0, source);

        while (!queue.empty()) {
            const auto [distance, current] = queue.top();
            queue.pop();

            if (distance != output.distances[current]) {
                continue;
            }

            for (std::uint64_t idx = offsets[current]; idx < offsets[current + 1]; ++idx) {
                const auto neighbor = neighbors[idx];
                const double candidate = distance + 1.0;

                if (candidate < output.distances[neighbor]) {
                    output.distances[neighbor] = candidate;
                    queue.emplace(candidate, neighbor);
                }
            }
        }

        std::uint64_t reached = 0;
        double max_distance = 0.0;

        for (const auto distance : output.distances) {
            if (std::isfinite(distance)) {
                ++reached;
                max_distance = std::max(max_distance, distance);
            }
        }

        output.summary.source = source;
        output.summary.reached_nodes = reached;
        output.summary.max_distance = max_distance;
        return output;
    });
}

} // namespace graphrush
