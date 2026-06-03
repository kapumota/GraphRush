#include "graphrush/CsrGraph.hpp"
#include "graphrush/GraphTypes.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

int main() {
    const char* input_path = "test_graph.edges";
    const char* output_path = "test_graph.grcsr";

    {
        std::ofstream output(input_path);
        output << "# Grafo de prueba\n";
        output << "1 2\n";
        output << "1 3\n";
        output << "2 4\n";
        output << "3 4\n";
        output << "3 4\n";
    }

    graphrush::GraphBuildOptions options;
    options.format = graphrush::GraphTextFormat::Snap;
    options.directed = true;
    options.deduplicate = true;

    auto graph = graphrush::CsrGraph::load_from_text(input_path, options);

    assert(graph->node_count() == 4);
    assert(graph->edge_count() == 4);
    assert(graph->max_degree() == 2);
    assert(graph->memory_bytes() > 0);
    assert(graph->validate());

    graph->write_binary(output_path);

    auto loaded = graphrush::CsrGraph::load_binary(output_path);
    assert(loaded->node_count() == graph->node_count());
    assert(loaded->edge_count() == graph->edge_count());
    assert(loaded->validate());

    std::cout << "Prueba Graph IO completada correctamente.\n";
    return 0;
}
