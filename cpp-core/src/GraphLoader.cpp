#include "graphrush/GraphLoader.hpp"

namespace graphrush {

std::unique_ptr<CsrGraph> GraphLoader::load_text(
    const std::string& path,
    const GraphBuildOptions& options
) {
    return CsrGraph::load_from_text(path, options);
}

std::unique_ptr<CsrGraph> GraphLoader::load_binary(const std::string& path) {
    return CsrGraph::load_binary(path);
}

} // namespace graphrush
