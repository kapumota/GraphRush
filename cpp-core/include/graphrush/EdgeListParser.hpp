#pragma once

#include <string>
#include <vector>

#include "graphrush/GraphTypes.hpp"

namespace graphrush {

class EdgeListParser {
public:
    static std::vector<WeightedEdge> parse(
        const std::string& path,
        GraphTextFormat format,
        bool weighted
    );
};

} // namespace graphrush
