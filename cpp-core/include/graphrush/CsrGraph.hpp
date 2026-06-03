#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace graphrush {

class CsrGraph {
public:
    static std::unique_ptr<CsrGraph> load_from_edge_list(const std::string& path);

    std::uint64_t node_count() const noexcept;
    std::uint64_t edge_count() const noexcept;
    std::uint64_t max_degree() const noexcept;
    double average_degree() const noexcept;

private:
    std::vector<std::uint64_t> offsets_;
    std::vector<std::uint64_t> neighbors_;

    explicit CsrGraph(
        std::vector<std::uint64_t> offsets,
        std::vector<std::uint64_t> neighbors
    );
};

} // namespace graphrush
