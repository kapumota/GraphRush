### GraphRush — Frontera FFI

#### Objetivo

Mantener una frontera segura, estable y de alto nivel entre Rust y C++.

#### Principio

Rust no cruza la frontera FFI por cada arista. Rust llama operaciones de alto nivel y el core C++ ejecuta el trabajo intensivo.

#### Operaciones expuestas

```text
load_graph
import_graph
graph_stats
run_bfs_report
run_components_report
run_pagerank_report
run_dijkstra_report
run_sssp_report
write_sssp_distances_csv
```

#### Tipos opacos

`CsrGraph` se trata como tipo opaco en Rust. El acceso a offsets, vecinos y pesos queda encapsulado en C++.

#### Ventaja

Esta frontera evita punteros crudos en Rust y mantiene el rendimiento dentro del core C++.
