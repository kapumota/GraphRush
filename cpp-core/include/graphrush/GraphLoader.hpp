#pragma once

#include <memory>
#include <string>

#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"

namespace graphrush {

class GraphLoader {
public:
    static std::unique_ptr<CsrGraph> load_text(
        const std::string& path,
        const GraphBuildOptions& options
    );

    static std::unique_ptr<CsrGraph> load_binary(const std::string& path);
};

} // namespace graphrush
