### Fase 2.1 — Baselines externos, exportación de resultados y grafos de control

#### Objetivo

Completar la Fase 2 con elementos profesionales de validación y trazabilidad antes de pasar al paralelismo.

#### Alcance

```text
scripts/compare_with_gapbs.md
docs/baseline_gapbs.md
más grafos de control
exportación de resultados a JSON
top-k PageRank
guardar distancias BFS/Dijkstra en CSV
```

#### Nuevos comandos

```bash
graphrush bfs --graph graph.gr --source 0 --json
graphrush bfs --graph graph.gr --source 0 --output-csv reports/bfs_distances.csv

graphrush components --graph graph.gr --json

graphrush pagerank --graph graph.gr --iterations 20 --top-k 20
graphrush pagerank --graph graph.gr --iterations 20 --top-k 20 --json

graphrush dijkstra --graph graph.gr --source 0 --json
graphrush dijkstra --graph graph.gr --source 0 --output-csv reports/dijkstra_distances.csv
```

#### Grafos de control

```text
data/control/line_6.edges
data/control/two_components.edges
data/control/directed_dag.edges
data/control/star_7.edges
data/control/cycle_6.edges
```

#### Criterio de éxito

La fase se considera completa cuando:

```text
los algoritmos generan salida humana
los algoritmos generan salida JSON
BFS exporta distancias a CSV
Dijkstra exporta distancias a CSV
PageRank muestra top-k
la comparación con GAPBS queda documentada
```
