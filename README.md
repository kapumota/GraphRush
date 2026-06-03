### GraphRush

#### Motor híbrido C++/Rust para análisis paralelo de grafos y ciberseguridad

GraphRush es un motor de análisis de grafos en memoria orientado a redes reales. El núcleo algorítmico se implementa en C++20 y la CLI se implementa en Rust. Esta versión incorpora la **Fase 1.1**, que cierra formalmente la etapa de carga de grafos antes de iniciar la Fase 2.

#### Estado de esta entrega

Esta entrega incluye:

```text
Fase 0: arquitectura híbrida C++/Rust y frontera FFI
Fase 1: carga de grafos reales y representación CSR
Fase 1.1: métricas completas, salida JSON y validación reforzada
```

#### Capacidades actuales

```text
GraphLoader
EdgeListParser
CSRGraph
GraphStats
BinaryGraphWriter
BinaryGraphReader
formato binario .grcsr
CLI Rust con import, stats y validate
salida humana y salida JSON
medición de memoria aproximada
medición de tiempo de carga
validación de rutas desde Rust
```

#### Comandos principales

```bash
cd rust-cli

cargo run -- import \
  --input ../data/small/example.edges \
  --format snap \
  --output ../data/small/example.grcsr \
  --directed \
  --deduplicate

cargo run -- stats \
  --graph ../data/small/example.grcsr

cargo run -- stats \
  --graph ../data/small/example.grcsr \
  --json

cargo run -- validate \
  --graph ../data/small/example.grcsr
```

#### Salida esperada de stats

```text
[GraphRush] Grafo cargado correctamente.
[GraphRush] Nodos: 6
[GraphRush] Aristas: 8
[GraphRush] Grado máximo: 2
[GraphRush] Grado promedio: 1.3333
[GraphRush] Memoria aproximada: 120 bytes
[GraphRush] Tiempo de carga: 0.12 ms
```

#### Fases del proyecto

| Fase | Nombre | Resultado |
|---:|---|---|
| 0 | Arquitectura híbrida C++/Rust y FFI | frontera técnica segura |
| 1 | GraphLoader + CSRGraph | carga de grafos reales |
| 1.1 | Métricas, JSON y validación | cierre profesional de ingesta |
| 2 | Algoritmos secuenciales | BFS, PageRank, componentes y Dijkstra |
| 3 | Paralelismo controlado | PageRank, BFS y componentes paralelos |
| 4 | Benchmark Engine | evaluación reproducible |
| 5 | Delta-Stepping SSSP | caminos mínimos paralelos |
| 6 | Security Pack | aplicación profesional en ciberseguridad |
| 7 | Release final | documentación, tests, benchmarks y demo |

#### Verificación de cumplimiento

El archivo `docs/phase1_compliance.md` resume el cumplimiento formal de la Fase 1.


#### Fase 2 agregada

La Fase 2 implementa los baselines secuenciales:

```text
BFS
Connected Components
PageRank
Dijkstra
```

Comandos:

```bash
graphrush bfs --graph data/small/example.grcsr --source 0
graphrush components --graph data/small/example.grcsr
graphrush pagerank --graph data/small/example.grcsr --iterations 20
graphrush dijkstra --graph data/small/example.grcsr --source 0
```


#### Fase 2.1 agregada

La Fase 2.1 completa los baselines con:

```text
comparación documentada con GAPBS
grafos de control adicionales
salida JSON para algoritmos
top-k PageRank
CSV de distancias BFS
CSV de distancias Dijkstra
```

Comandos:

```bash
graphrush bfs --graph data/small/example.grcsr --source 0 --json
graphrush bfs --graph data/small/example.grcsr --source 0 --output-csv reports/bfs_distances.csv
graphrush pagerank --graph data/small/example.grcsr --iterations 20 --top-k 5 --json
graphrush dijkstra --graph data/small/example.grcsr --source 0 --output-csv reports/dijkstra_distances.csv
```


#### Fase 3 agregada

La Fase 3 incorpora paralelismo controlado con OpenMP:

```text
BFS paralelo por frontier
Connected Components paralelo por label propagation
PageRank paralelo con OpenMP
```

Comandos:

```bash
graphrush parallel-bfs --graph data/small/example.grcsr --source 0 --threads 8
graphrush parallel-components --graph data/small/example.grcsr --threads 8
graphrush parallel-pagerank --graph data/small/example.grcsr --iterations 20 --threads 8 --top-k 5
```


#### Fase 3.1 agregada

La Fase 3.1 unifica comandos con `--threads` y agrega benchmark de paralelismo.

```bash
graphrush pagerank --graph data/small/example.grcsr --iterations 30 --threads 8
graphrush bfs --graph data/small/example.grcsr --source 0 --threads 8
graphrush components --graph data/small/example.grcsr --threads 8
```


#### Fase 3.2 agregada

La Fase 3.2 integra `/usr/bin/time -v` en `scripts/phase3_parallel_benchmark.py` para llenar automáticamente `memory_peak_kb`.

```bash
python scripts/phase3_parallel_benchmark.py \
  --binary ./target/release/graphrush \
  --graph data/web-google.gr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --measure-memory
```
