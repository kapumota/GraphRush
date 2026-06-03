#include "graphrush/Algorithms.hpp"
#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"
#include "graphrush/ParallelAlgorithms.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>

int main() {
    const char* input_path = "test_parallel.edges";

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

    const auto bfs_seq = graphrush::SequentialAlgorithms::bfs(*graph, 0);
    const auto bfs_par = graphrush::ParallelAlgorithms::bfs_frontier(*graph, 0, 4);
    assert(bfs_seq.summary.reached_nodes == bfs_par.summary.reached_nodes);
    assert(bfs_seq.summary.max_depth == bfs_par.summary.max_depth);
    assert(bfs_seq.distances == bfs_par.distances);

    const auto pr_seq = graphrush::SequentialAlgorithms::pagerank(*graph, 10, 0.85);
    const auto pr_par = graphrush::ParallelAlgorithms::pagerank_pull_atomic(*graph, 10, 0.85, 4);

    double sum_seq = 0.0;
    double sum_par = 0.0;
    for (double value : pr_seq.scores) {
        sum_seq += value;
    }
    for (double value : pr_par.scores) {
        sum_par += value;
    }

    assert(std::abs(sum_seq - 1.0) < 1e-9);
    assert(std::abs(sum_par - 1.0) < 1e-9);

    // Para componentes, se importa como no dirigido para evaluar conectividad débil.
    graphrush::GraphBuildOptions undirected_options;
    undirected_options.format = graphrush::GraphTextFormat::EdgeList;
    undirected_options.directed = false;
    undirected_options.deduplicate = true;

    auto undirected_graph = graphrush::CsrGraph::load_from_text(input_path, undirected_options);
    const auto cc_seq = graphrush::SequentialAlgorithms::connected_components(*undirected_graph);
    const auto cc_par =
        graphrush::ParallelAlgorithms::connected_components_label_propagation(*undirected_graph, 4);

    assert(cc_seq.summary.components == cc_par.summary.components);
    assert(cc_seq.summary.largest_component == cc_par.summary.largest_component);

    std::cout << "Pruebas de algoritmos paralelos completadas correctamente.\n";
    return 0;
}
