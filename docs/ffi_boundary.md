### Frontera FFI de GraphRush

#### Objetivo

Definir una frontera segura y mantenible entre Rust y C++.

#### Principio principal

La frontera FFI debe ser gruesa, no fina. Rust debe llamar funciones completas y C++ debe ejecutar el trabajo intensivo.

#### Correcto

```text
run_pagerank(graph, iterations, threads, damping)
run_bfs(graph, source, threads)
load_graph(path)
```

#### Incorrecto

```text
get_neighbor(graph, node, index)
next_edge(iterator)
process_edge_from_rust(edge)
```

Cruzar la frontera por cada arista destruiría el rendimiento y complicaría la seguridad.

#### Tipo opaco principal

```text
CsrGraph
```

Rust recibe un `UniquePtr<CsrGraph>`, pero no accede a sus campos internos.

#### API mínima de Fase 0

```text
load_graph(path)
node_count(graph)
edge_count(graph)
max_degree(graph)
average_degree(graph)
```

#### API futura

```text
run_bfs(graph, source, threads)
run_pagerank(graph, iterations, threads, damping)
run_components(graph, threads)
run_dijkstra(graph, source)
run_delta_stepping(graph, source, delta, threads)
```
