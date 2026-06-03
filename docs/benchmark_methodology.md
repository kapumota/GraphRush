### Metodología de benchmark

#### Referencias

```text
Graph500: TEPS/GTEPS para BFS y SSSP
GAP Benchmark Suite: metodología estandarizada, kernels y baselines optimizados
ChunkGraph: datasets grandes y evaluación moderna sobre grafos reales
```

#### Repeticiones

Por defecto:

```text
warmup = 1
runs = 4
reported_runs = últimas 3 ejecuciones
```

#### Métricas

| Métrica | Descripción |
|---|---|
| tiempo total | suma de todos los runs |
| average_time_ms | promedio reportado |
| speedup | tiempo_1_thread / tiempo_N_threads |
| parallel_efficiency | speedup / N |
| edges_per_second | aristas / tiempo_segundos |
| gteps | edges_per_second / 1e9 |
| memory_peak_kb | Maximum resident set size |
| cache_misses | medido con perf si está disponible |

#### Datasets recomendados

```text
ego-Facebook
wiki-Vote
soc-Epinions1
web-Google
roadNet-CA
Graph500 Kronecker sintético
```

#### Nota

Friendster, Twitter, UKdomain y Yahoo Web son útiles como datasets de estrés, pero no deben ser obligatorios para desarrollo inicial porque pueden requerir hardware considerable.
