### Frontera FFI de GraphRush

#### Objetivo

Mantener una frontera segura y de alto nivel entre Rust y C++.

#### Principio principal

Rust invoca operaciones completas y C++ ejecuta el trabajo intensivo.

#### API actual

```text
load_graph(path)
import_graph(input, output, format, directed, deduplicate)
node_count(graph)
edge_count(graph)
max_degree(graph)
average_degree(graph)
memory_bytes(graph)
validate_graph(graph)
```

#### API futura

```text
run_bfs(graph, source, threads)
run_pagerank(graph, iterations, threads, damping)
run_components(graph, threads)
run_dijkstra(graph, source)
run_delta_stepping(graph, source, delta, threads)
```

#### Regla de rendimiento

No se debe cruzar la frontera FFI por cada arista. Eso degradaría el rendimiento y haría más frágil el diseño.
