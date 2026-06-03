### Fase 2 — Algoritmos secuenciales de referencia

#### Objetivo

Implementar versiones correctas y verificables antes de paralelizar. Esta fase crea los baselines internos de GraphRush para comparar las fases paralelas posteriores.

#### Algoritmos

```text
BFS secuencial
Connected Components secuencial
PageRank secuencial
Dijkstra secuencial
```

#### Comandos

```bash
graphrush bfs --graph web-google.gr --source 0
graphrush components --graph web-google.gr
graphrush pagerank --graph web-google.gr --iterations 20
graphrush dijkstra --graph road-ca.gr --source 0
```

#### Criterio de éxito

Los resultados deben coincidir con grafos pequeños de control y con implementaciones baseline.

#### Referencia metodológica

GraphRush usa GAP Benchmark Suite como referencia de metodología y comparación externa. GAPBS estandariza kernels, grafos de entrada y metodología de evaluación, además de proveer implementaciones de referencia optimizadas.

#### Decisión de implementación

GraphRush no copia los algoritmos de GAPBS. Implementa versiones secuenciales propias, simples y verificables:

- BFS con cola FIFO.
- Connected Components mediante recorridos BFS sobre el grafo almacenado.
- PageRank iterativo secuencial con manejo de nodos dangling.
- Dijkstra con cola de prioridad y peso unitario por defecto.

#### Nota sobre componentes

En esta fase, `components` analiza la conectividad según las aristas almacenadas. Para grafos no dirigidos, se recomienda importar con:

```bash
graphrush import --input graph.txt --format snap --output graph.gr --directed false
```

#### Nota sobre Dijkstra

La Fase 2 usa peso unitario por arista. Eso permite validar la ruta de SSSP antes de agregar pesos reales o Delta-Stepping en fases posteriores.

#### Tests

```text
test_sequential_algorithms.cpp
```

Este test valida BFS, componentes, PageRank y Dijkstra sobre un grafo pequeño de control.
