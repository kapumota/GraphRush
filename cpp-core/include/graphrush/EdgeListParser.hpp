#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "graphrush/GraphTypes.hpp"

namespace graphrush {

class EdgeListParser {
public:
    static std::vector<std::pair<std::uint64_t, std::uint64_t>> parse(
        const std::string& path,
        GraphTextFormat format
    );
};

} // namespace graphrush
