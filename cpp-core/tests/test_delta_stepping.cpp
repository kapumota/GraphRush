#include "graphrush/Algorithms.hpp"
#include "graphrush/CsrGraph.hpp"
#include "graphrush/DeltaStepping.hpp"
#include "graphrush/GraphTypes.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

int main() {
    const char* input_path = "test_sssp.edges";

    {
        std::ofstream output(input_path);
        output << "0 1\n";
        output << "0 2\n";
        output << "1 3\n";
        output << "2 3\n";
        output << "3 4\n";
        output << "4 5\n";
        output << "5 6\n";
        output << "2 6\n";
    }

    graphrush::GraphBuildOptions options;
    options.format = graphrush::GraphTextFormat::EdgeList;
    options.directed = true;
    options.deduplicate = true;

    auto graph = graphrush::CsrGraph::load_from_text(input_path, options);

    const auto dijkstra = graphrush::SequentialAlgorithms::dijkstra(*graph, 0);

    graphrush::DeltaSteppingConfig config;
    config.delta = 4.0;
    config.threads = 4;

    const auto delta = graphrush::DeltaStepping::sssp(*graph, 0, config);

    assert(dijkstra.summary.reached_nodes == delta.summary.reached_nodes);
    assert(std::abs(dijkstra.summary.max_distance - delta.summary.max_distance) < 1e-9);
    assert(dijkstra.distances.size() == delta.distances.size());

    for (std::size_t i = 0; i < dijkstra.distances.size(); ++i) {
        if (std::isfinite(dijkstra.distances[i])) {
            assert(std::abs(dijkstra.distances[i] - delta.distances[i]) < 1e-9);
        } else {
            assert(!std::isfinite(delta.distances[i]));
        }
    }

    std::cout << "Prueba Delta-Stepping completada correctamente.\n";
    return 0;
}
