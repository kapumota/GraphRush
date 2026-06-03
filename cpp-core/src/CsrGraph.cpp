#include "graphrush/CsrGraph.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <utility>

namespace graphrush {

CsrGraph::CsrGraph(
    std::vector<std::uint64_t> offsets,
    std::vector<std::uint64_t> neighbors
)
    : offsets_(std::move(offsets)),
      neighbors_(std::move(neighbors)) {}

std::unique_ptr<CsrGraph> CsrGraph::load_from_edge_list(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo de grafo: " + path);
    }

    std::unordered_map<std::uint64_t, std::uint64_t> remap;
    std::vector<std::pair<std::uint64_t, std::uint64_t>> edges;

    auto get_internal_id = [&remap](std::uint64_t external_id) -> std::uint64_t {
        auto it = remap.find(external_id);
        if (it != remap.end()) {
            return it->second;
        }

        const std::uint64_t new_id = static_cast<std::uint64_t>(remap.size());
        remap.emplace(external_id, new_id);
        return new_id;
    };

    std::string line;
    while (std::getline(input, line)) {
        // Ignora líneas vacías y comentarios estilo SNAP.
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::uint64_t src = 0;
        std::uint64_t dst = 0;

        if (!(iss >> src >> dst)) {
            continue;
        }

        const std::uint64_t u = get_internal_id(src);
        const std::uint64_t v = get_internal_id(dst);
        edges.emplace_back(u, v);
    }

    const std::uint64_t nodes = static_cast<std::uint64_t>(remap.size());
    std::sort(edges.begin(), edges.end());

    std::vector<std::uint64_t> offsets(nodes + 1, 0);
    for (const auto& edge : edges) {
        offsets[edge.first + 1] += 1;
    }

    for (std::uint64_t i = 1; i < offsets.size(); ++i) {
        offsets[i] += offsets[i - 1];
    }

    std::vector<std::uint64_t> cursor = offsets;
    std::vector<std::uint64_t> neighbors(edges.size(), 0);

    for (const auto& edge : edges) {
        const std::uint64_t position = cursor[edge.first]++;
        neighbors[position] = edge.second;
    }

    return std::unique_ptr<CsrGraph>(
        new CsrGraph(std::move(offsets), std::move(neighbors))
    );
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

} // namespace graphrush
