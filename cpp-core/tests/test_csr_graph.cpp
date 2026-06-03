#include "graphrush/CsrGraph.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

int main() {
    const char* path = "test_graph.edges";

    {
        std::ofstream output(path);
        output << "1 2\n";
        output << "1 3\n";
        output << "2 4\n";
        output << "3 4\n";
    }

    const auto graph = graphrush::CsrGraph::load_from_edge_list(path);

    assert(graph->node_count() == 4);
    assert(graph->edge_count() == 4);
    assert(graph->max_degree() == 2);

    std::cout << "Prueba CSRGraph completada correctamente.\n";
    return 0;
}
