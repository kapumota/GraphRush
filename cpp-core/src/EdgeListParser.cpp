#include "graphrush/EdgeListParser.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace graphrush {

std::vector<WeightedEdge> EdgeListParser::parse(
    const std::string& path,
    GraphTextFormat format,
    bool weighted
) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo de entrada: " + path);
    }

    std::vector<WeightedEdge> edges;
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
        WeightedEdge edge;

        if (!(iss >> edge.source >> edge.target)) {
            continue;
        }

        if (weighted) {
            if (!(iss >> edge.weight)) {
                edge.weight = 1.0;
            }
        } else {
            edge.weight = 1.0;
        }

        if (edge.weight < 0.0) {
            throw std::runtime_error("GraphRush no acepta pesos negativos para SSSP.");
        }

        edges.push_back(edge);
    }

    return edges;
}

} // namespace graphrush
