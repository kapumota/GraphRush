#include "graphrush/Algorithms.hpp"
#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

int main() {
    const char* input_path = "test_algorithms.edges";

    {
        std::ofstream output(input_path);
        output << "0 1\n";
        output << "0 2\n";
        output << "1 3\n";
        output << "2 3\n";
        output << "3 4\n";
        output << "4 5\n";
        output << "5 0\n";
    }

    graphrush::GraphBuildOptions options;
    options.format = graphrush::GraphTextFormat::EdgeList;
    options.directed = true;
    options.deduplicate = true;

    auto graph = graphrush::CsrGraph::load_from_text(input_path, options);
    assert(graph->validate());

    const auto bfs = graphrush::SequentialAlgorithms::bfs(*graph, 0);
    assert(bfs.summary.reached_nodes == 6);
    assert(bfs.summary.max_depth == 4);
    assert(bfs.distances[0] == 0);
    assert(bfs.distances[3] == 2);

    const auto components = graphrush::SequentialAlgorithms::connected_components(*graph);
    assert(components.summary.components == 1);
    assert(components.summary.largest_component == 6);

    const auto pagerank = graphrush::SequentialAlgorithms::pagerank(*graph, 20, 0.85);
    double sum = 0.0;
    for (double score : pagerank.scores) {
        assert(score >= 0.0);
        sum += score;
    }
    assert(std::abs(sum - 1.0) < 1e-9);

    const auto dijkstra = graphrush::SequentialAlgorithms::dijkstra(*graph, 0);
    assert(dijkstra.summary.reached_nodes == 6);
    assert(std::abs(dijkstra.distances[5] - 4.0) < 1e-9);

    std::cout << "Pruebas de algoritmos secuenciales completadas correctamente.\n";
    return 0;
}
