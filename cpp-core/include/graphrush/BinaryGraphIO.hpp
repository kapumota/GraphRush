#pragma once

#include <memory>
#include <string>

#include "graphrush/CsrGraph.hpp"

namespace graphrush {

class BinaryGraphWriter {
public:
    static void write(const CsrGraph& graph, const std::string& path);
};

class BinaryGraphReader {
public:
    static std::unique_ptr<CsrGraph> read(const std::string& path);
};

} // namespace graphrush
