#include "graphrush/CsrGraph.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>
#include <unordered_map>
#include <utility>

#include "graphrush/BinaryGraphIO.hpp"
#include "graphrush/EdgeListParser.hpp"

namespace graphrush {

CsrGraph::CsrGraph(
    std::vector<std::uint64_t> offsets,
    std::vector<std::uint64_t> neighbors,
    std::vector<double> weights
)
    : offsets_(std::move(offsets)),
      neighbors_(std::move(neighbors)),
      weights_(std::move(weights)) {}

std::unique_ptr<CsrGraph> CsrGraph::load_from_text(
    const std::string& path,
    const GraphBuildOptions& options
) {
    auto input_edges = EdgeListParser::parse(path, options.format, options.weighted);

    if (!options.directed) {
        const std::size_t original_size = input_edges.size();
        input_edges.reserve(original_size * 2);
        for (std::size_t i = 0; i < original_size; ++i) {
            WeightedEdge reverse;
            reverse.source = input_edges[i].target;
            reverse.target = input_edges[i].source;
            reverse.weight = input_edges[i].weight;
            input_edges.push_back(reverse);
        }
    }

    std::unordered_map<std::uint64_t, std::uint64_t> remap;

    auto get_internal_id = [&remap](std::uint64_t external_id) -> std::uint64_t {
        auto it = remap.find(external_id);
        if (it != remap.end()) {
            return it->second;
        }

        const std::uint64_t new_id = static_cast<std::uint64_t>(remap.size());
        remap.emplace(external_id, new_id);
        return new_id;
    };

    std::vector<WeightedEdge> normalized;
    normalized.reserve(input_edges.size());

    for (const auto& edge : input_edges) {
        WeightedEdge mapped;
        mapped.source = get_internal_id(edge.source);
        mapped.target = get_internal_id(edge.target);
        mapped.weight = edge.weight;
        normalized.push_back(mapped);
    }

    std::sort(normalized.begin(), normalized.end(), [](const auto& a, const auto& b) {
        if (a.source != b.source) {
            return a.source < b.source;
        }
        if (a.target != b.target) {
            return a.target < b.target;
        }
        return a.weight < b.weight;
    });

    if (options.deduplicate) {
        std::vector<WeightedEdge> deduplicated;
        deduplicated.reserve(normalized.size());

        for (const auto& edge : normalized) {
            if (!deduplicated.empty() &&
                deduplicated.back().source == edge.source &&
                deduplicated.back().target == edge.target) {
                // Conserva el menor peso entre aristas duplicadas.
                deduplicated.back().weight = std::min(deduplicated.back().weight, edge.weight);
            } else {
                deduplicated.push_back(edge);
            }
        }

        normalized.swap(deduplicated);
    }

    const std::uint64_t nodes = static_cast<std::uint64_t>(remap.size());
    std::vector<std::uint64_t> offsets(nodes + 1, 0);

    for (const auto& edge : normalized) {
        offsets[edge.source + 1] += 1;
    }

    for (std::uint64_t i = 1; i < offsets.size(); ++i) {
        offsets[i] += offsets[i - 1];
    }

    std::vector<std::uint64_t> cursor = offsets;
    std::vector<std::uint64_t> neighbors(normalized.size(), 0);
    std::vector<double> weights;
    if (options.weighted) {
        weights.assign(normalized.size(), 1.0);
    }

    for (const auto& edge : normalized) {
        const std::uint64_t position = cursor[edge.source]++;
        neighbors[position] = edge.target;
        if (options.weighted) {
            weights[position] = edge.weight;
        }
    }

    return std::make_unique<CsrGraph>(
        std::move(offsets),
        std::move(neighbors),
        std::move(weights)
    );
}

std::unique_ptr<CsrGraph> CsrGraph::load_binary(const std::string& path) {
    return BinaryGraphReader::read(path);
}

void CsrGraph::write_binary(const std::string& path) const {
    BinaryGraphWriter::write(*this, path);
}

std::uint64_t CsrGraph::node_count() const noexcept {
    if (offsets_.empty()) {
        return 0;
    }
    return static_cast<std::uint64_t>(offsets_.size() - 1);
}

std::uint64_t CsrGraph::edge_count() const noexcept {
    return static_cast<std::uint64_t>(neighbors_.size());
}

std::uint64_t CsrGraph::max_degree() const noexcept {
    std::uint64_t best = 0;

    for (std::uint64_t i = 0; i + 1 < offsets_.size(); ++i) {
        const std::uint64_t degree = offsets_[i + 1] - offsets_[i];
        best = std::max(best, degree);
    }

    return best;
}

double CsrGraph::average_degree() const noexcept {
    const std::uint64_t nodes = node_count();
    if (nodes == 0) {
        return 0.0;
    }

    return static_cast<double>(edge_count()) / static_cast<double>(nodes);
}

std::uint64_t CsrGraph::memory_bytes() const noexcept {
    return static_cast<std::uint64_t>(
        offsets_.size() * sizeof(std::uint64_t) +
        neighbors_.size() * sizeof(std::uint64_t) +
        weights_.size() * sizeof(double)
    );
}

bool CsrGraph::has_weights() const noexcept {
    return !weights_.empty();
}

double CsrGraph::edge_weight(std::uint64_t edge_index) const noexcept {
    if (weights_.empty()) {
        return 1.0;
    }
    return weights_[edge_index];
}

bool CsrGraph::validate() const noexcept {
    if (offsets_.empty()) {
        return neighbors_.empty() && weights_.empty();
    }

    if (offsets_.front() != 0) {
        return false;
    }

    if (offsets_.back() != neighbors_.size()) {
        return false;
    }

    if (!weights_.empty() && weights_.size() != neighbors_.size()) {
        return false;
    }

    for (std::size_t i = 1; i < offsets_.size(); ++i) {
        if (offsets_[i] < offsets_[i - 1]) {
            return false;
        }
    }

    const std::uint64_t nodes = node_count();
    for (const auto neighbor : neighbors_) {
        if (neighbor >= nodes) {
            return false;
        }
    }

    for (const auto weight : weights_) {
        if (!std::isfinite(weight) || weight < 0.0) {
            return false;
        }
    }

    return true;
}

CoreGraphStats CsrGraph::stats(double load_time_ms) const noexcept {
    CoreGraphStats result;
    result.nodes = node_count();
    result.edges = edge_count();
    result.max_degree = max_degree();
    result.average_degree = average_degree();
    result.memory_bytes = memory_bytes();
    result.load_time_ms = load_time_ms;
    result.valid = validate();
    result.weighted = has_weights();
    return result;
}

const std::vector<std::uint64_t>& CsrGraph::offsets() const noexcept {
    return offsets_;
}

const std::vector<std::uint64_t>& CsrGraph::neighbors() const noexcept {
    return neighbors_;
}

const std::vector<double>& CsrGraph::weights() const noexcept {
    return weights_;
}

} // namespace graphrush
