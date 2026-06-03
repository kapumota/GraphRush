### GraphRush — Benchmarks

#### Objetivo

Medir rendimiento de GraphRush con metodología reproducible.

#### Métricas

```text
tiempo total
average_time_ms
throughput en aristas/segundo
GTEPS
speedup
eficiencia paralela
memoria pico
cache misses si se usa perf
```

#### Metodología

GraphRush adopta una metodología inspirada en GAP Benchmark Suite:

```text
warmup separado
4 runs por configuración
promedio reportado sobre las últimas 3 ejecuciones
threads 1, 2, 4, 8 y 16
```

#### Comando principal

```bash
graphrush benchmark \
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

#### Nota

Para grafos pequeños, el speedup puede ser bajo por overhead de paralelización. La evaluación real debe hacerse con grafos medianos o grandes.
