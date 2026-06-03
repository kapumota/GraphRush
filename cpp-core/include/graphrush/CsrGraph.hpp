#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "graphrush/GraphTypes.hpp"

namespace graphrush {

class CsrGraph {
public:
    CsrGraph(
        std::vector<std::uint64_t> offsets,
        std::vector<std::uint64_t> neighbors
    );

    static std::unique_ptr<CsrGraph> load_from_text(
        const std::string& path,
        const GraphBuildOptions& options
    );

    static std::unique_ptr<CsrGraph> load_binary(const std::string& path);

    void write_binary(const std::string& path) const;

    std::uint64_t node_count() const noexcept;
    std::uint64_t edge_count() const noexcept;
    std::uint64_t max_degree() const noexcept;
    double average_degree() const noexcept;
    std::uint64_t memory_bytes() const noexcept;
    bool validate() const noexcept;
    CoreGraphStats stats(double load_time_ms = 0.0) const noexcept;

    const std::vector<std::uint64_t>& offsets() const noexcept;
    const std::vector<std::uint64_t>& neighbors() const noexcept;

private:
    std::vector<std::uint64_t> offsets_;
    std::vector<std::uint64_t> neighbors_;
};

} // namespace graphrush
