### Fase 4 - Benchmark Engine y evaluación reproducible

#### Objetivo

Convertir GraphRush en software evaluable profesionalmente, no solo en una implementación de algoritmos.

#### Componentes

```text
BenchmarkRunner
MetricsCollector
CSVReporter
JSONReporter
MarkdownReporter
PerfSummaryReporter
```

#### Comando oficial

```bash
graphrush benchmark \
  --graph web-google.gr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --output reports/
```

#### Comando equivalente con script

```bash
python scripts/benchmark_engine.py \
  --binary ./target/release/graphrush \
  --graph web-google.gr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --output reports/
```

#### Salidas

```text
reports/benchmark.csv
reports/benchmark.json
reports/benchmark_results.md
reports/perf_summary.md
```

#### Métricas obligatorias

```text
tiempo total
throughput en aristas/segundo
GTEPS
speedup
eficiencia paralela
memoria usada
cache misses si se usa perf
```

#### Metodología

GraphRush adopta una metodología inspirada en GAP Benchmark Suite:

```text
ejecutar 4 runs por configuración
reportar el promedio de las últimas 3 ejecuciones
separar warmup de medición
comparar 1, 2, 4, 8 y 16 threads
```

#### Graph500

GraphRush agrega `GTEPS` para que BFS y recorridos puedan compararse con la tradición de Graph500:

```text
GTEPS = aristas_procesadas / tiempo_segundos / 1_000_000_000
```

#### Perf

Si se usa:

```bash
--use-perf
```

el runner intenta ejecutar:

```bash
perf stat -e cache-references,cache-misses
```

y genera `reports/perf_summary.md`.
