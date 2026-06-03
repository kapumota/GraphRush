### Cumplimiento de Fase 2

#### Objetivo

Implementar versiones correctas y verificables antes de paralelizar.

#### Algoritmos

| Algoritmo | Estado | Archivo principal |
|---|---:|---|
| BFS secuencial | Cumple | `cpp-core/src/Algorithms.cpp` |
| Connected Components secuencial | Cumple | `cpp-core/src/Algorithms.cpp` |
| PageRank secuencial | Cumple | `cpp-core/src/Algorithms.cpp` |
| Dijkstra secuencial | Cumple | `cpp-core/src/Algorithms.cpp` |

#### Comandos

| Comando | Estado |
|---|---:|
| `graphrush bfs --graph web-google.gr --source 0` | Cumple |
| `graphrush components --graph web-google.gr` | Cumple |
| `graphrush pagerank --graph web-google.gr --iterations 20` | Cumple |
| `graphrush dijkstra --graph road-ca.gr --source 0` | Cumple |

#### Criterio de éxito

Los algoritmos se validan con `test_sequential_algorithms.cpp`, comparando resultados esperados en un grafo pequeño de control.

#### Baseline externo

GAP Benchmark Suite se usa como referencia externa de metodología, kernels y evaluación reproducible. GraphRush no copia su implementación; usa sus ideas como estándar de comparación para las fases 2 y 3.
