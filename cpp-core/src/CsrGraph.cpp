#include "graphrush/CsrGraph.hpp"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "graphrush/BinaryGraphIO.hpp"
#include "graphrush/EdgeListParser.hpp"

namespace graphrush {

CsrGraph::CsrGraph(
    std::vector<std::uint64_t> offsets,
    std::vector<std::uint64_t> neighbors
)
    : offsets_(std::move(offsets)),
      neighbors_(std::move(neighbors)) {}

std::unique_ptr<CsrGraph> CsrGraph::load_from_text(
    const std::string& path,
    const GraphBuildOptions& options
) {
    auto edges = EdgeListParser::parse(path, options.format);

    if (!options.directed) {
        const std::size_t original_size = edges.size();
        edges.reserve(original_size * 2);
        for (std::size_t i = 0; i < original_size; ++i) {
            edges.emplace_back(edges[i].second, edges[i].first);
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

    std::vector<std::pair<std::uint64_t, std::uint64_t>> normalized;
    normalized.reserve(edges.size());

    for (const auto& edge : edges) {
        normalized.emplace_back(get_internal_id(edge.first), get_internal_id(edge.second));
    }

    std::sort(normalized.begin(), normalized.end());

    if (options.deduplicate) {
        normalized.erase(
            std::unique(normalized.begin(), normalized.end()),
            normalized.end()
        );
    }

    const std::uint64_t nodes = static_cast<std::uint64_t>(remap.size());
    std::vector<std::uint64_t> offsets(nodes + 1, 0);

    for (const auto& edge : normalized) {
        offsets[edge.first + 1] += 1;
    }

    for (std::uint64_t i = 1; i < offsets.size(); ++i) {
        offsets[i] += offsets[i - 1];
    }

    std::vector<std::uint64_t> cursor = offsets;
    std::vector<std::uint64_t> neighbors(normalized.size(), 0);

    for (const auto& edge : normalized) {
        const std::uint64_t position = cursor[edge.first]++;
        neighbors[position] = edge.second;
    }

    return std::make_unique<CsrGraph>(std::move(offsets), std::move(neighbors));
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
        if (degree > best) {
            best = degree;
        }
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
        neighbors_.size() * sizeof(std::uint64_t)
    );
}

bool CsrGraph::validate() const noexcept {
    if (offsets_.empty()) {
        return neighbors_.empty();
    }

    if (offsets_.front() != 0) {
        return false;
    }

    if (offsets_.back() != neighbors_.size()) {
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

    return true;
}

const std::vector<std::uint64_t>& CsrGraph::offsets() const noexcept {
    return offsets_;
}

const std::vector<std::uint64_t>& CsrGraph::neighbors() const noexcept {
    return neighbors_;
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
    return result;
}

} // namespace graphrush
