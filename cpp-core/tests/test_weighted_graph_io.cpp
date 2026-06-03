#include "graphrush/Algorithms.hpp"
#include "graphrush/CsrGraph.hpp"
#include "graphrush/DeltaStepping.hpp"
#include "graphrush/GraphTypes.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

int main() {
    const char* input_path = "test_weighted.edges";
    const char* output_path = "test_weighted.grcsr";

    {
        std::ofstream output(input_path);
        output << "0 1 2.5\n";
        output << "0 2 1.0\n";
        output << "2 1 0.5\n";
        output << "1 3 2.0\n";
        output << "2 3 5.0\n";
    }

    graphrush::GraphBuildOptions options;
    options.format = graphrush::GraphTextFormat::EdgeList;
    options.directed = true;
    options.deduplicate = true;
    options.weighted = true;

    auto graph = graphrush::CsrGraph::load_from_text(input_path, options);
    assert(graph->validate());
    assert(graph->has_weights());
    assert(graph->weights().size() == graph->edge_count());

    graph->write_binary(output_path);
    auto loaded = graphrush::CsrGraph::load_binary(output_path);
    assert(loaded->validate());
    assert(loaded->has_weights());
    assert(loaded->weights().size() == loaded->edge_count());

    const auto dijkstra = graphrush::SequentialAlgorithms::dijkstra(*loaded, 0);

    graphrush::DeltaSteppingConfig config;
    config.delta = 2.0;
    config.threads = 4;

    const auto delta = graphrush::DeltaStepping::sssp(*loaded, 0, config);

    assert(dijkstra.distances.size() == delta.distances.size());
    for (std::size_t i = 0; i < dijkstra.distances.size(); ++i) {
        assert(std::abs(dijkstra.distances[i] - delta.distances[i]) < 1e-9);
    }

    assert(std::abs(dijkstra.distances[1] - 1.5) < 1e-9);
    assert(std::abs(dijkstra.distances[3] - 3.5) < 1e-9);

    std::cout << "Prueba Weighted SSSP completada correctamente.\n";
    return 0;
}
