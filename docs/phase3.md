### Fase 3 — Paralelismo controlado

#### Objetivo

Agregar paralelismo de memoria compartida sobre los algoritmos ya validados en Fase 2, manteniendo una comparación directa contra los baselines secuenciales.

#### Decisión técnica

GraphRush usa OpenMP sobre C++20 como primera tecnología de paralelismo. Esta decisión mantiene el proyecto simple, portable y alineado con GAP Benchmark Suite, que también usa OpenMP como mecanismo de paralelismo.

#### Referencias de diseño

```text
Ligra / Ligra+: paralelismo de frontera, edgeMap, vertexMap
GAPBS: metodología de evaluación, kernels y OpenMP
NWGraph + HPX: ruta futura para ejecución distribuida
DCSR: referencia futura para grafos dinámicos, no necesaria en CSR estático
```

#### Algoritmos paralelos incluidos

```text
BFS paralelo por frontier
PageRank paralelo con OpenMP
Connected Components paralelo por label propagation
```

#### Comandos

```bash
graphrush parallel-bfs --graph web-google.gr --source 0 --threads 8
graphrush parallel-components --graph web-google.gr --threads 8
graphrush parallel-pagerank --graph web-google.gr --iterations 20 --threads 8 --top-k 20
```

#### Criterio de éxito

Los resultados agregados de los algoritmos paralelos deben coincidir con los secuenciales:

```text
BFS: mismos nodos alcanzados y misma profundidad máxima
Connected Components: mismo número de componentes y tamaño de componente mayor
PageRank: suma de scores cercana a 1.0 y ranking comparable
```

#### Alcance

Esta fase no implementa paralelismo distribuido ni grafos dinámicos. NWGraph + HPX y DCSR quedan documentados como evolución futura.

#### Nota sobre Connected Components

Para componentes conexas se recomienda importar el grafo como no dirigido:

```bash
graphrush import --input graph.txt --format snap --output graph.gr --directed false
```
