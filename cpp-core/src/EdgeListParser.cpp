#include "graphrush/EdgeListParser.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace graphrush {

std::vector<std::pair<std::uint64_t, std::uint64_t>> EdgeListParser::parse(
    const std::string& path,
    GraphTextFormat format
) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + path);
    }

    std::vector<std::pair<std::uint64_t, std::uint64_t>> edges;
    std::string line;

    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (format == GraphTextFormat::Csv) {
            for (char& ch : line) {
                if (ch == ',') {
                    ch = ' ';
                }
            }
        }

        std::istringstream iss(line);
        std::uint64_t src = 0;
        std::uint64_t dst = 0;

        if (!(iss >> src >> dst)) {
            continue;
        }

        edges.emplace_back(src, dst);
    }

    return edges;
}

} // namespace graphrush
