### Fase 3.1 — Benchmark de paralelismo y comandos unificados

#### Objetivo

Cerrar la Fase 3 agregando comandos unificados con `--threads`, mediciones de paralelismo y reportes reproducibles.

#### Comandos unificados

```bash
graphrush pagerank --graph web-google.gr --iterations 30 --threads 8
graphrush bfs --graph web-google.gr --source 42 --threads 8
graphrush components --graph web-google.gr --threads 8
```

Si `--threads` es mayor que 1, la CLI usa la versión paralela. Si `--threads` es 1, usa el baseline secuencial.

#### Benchmark de paralelismo

```bash
python scripts/phase3_parallel_benchmark.py \
  --binary ./target/release/graphrush \
  --graph data/web-google.gr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --source 42 \
  --iterations 30 \
  --output reports/phase3
```

#### Métricas

```text
tiempo con 1, 2, 4, 8 y 16 threads
speedup
eficiencia paralela
aristas procesadas por segundo
memoria pico aproximada
```

#### Fórmulas

```text
speedup = tiempo_1_hilo / tiempo_N_hilos
eficiencia = speedup / N
aristas_por_segundo = número_de_aristas / tiempo_en_segundos
```

#### Salidas

```text
reports/phase3/parallel_benchmark.csv
reports/phase3/parallel_benchmark.json
reports/phase3/parallel_benchmark.md
```

#### Nota sobre memoria pico

El script deja preparada la columna `memory_peak_kb`. Para una medición estricta se puede integrar `/usr/bin/time -v` o profiling en la Fase 4.


#### Actualización por Fase 3.2

La columna `memory_peak_kb` ahora se llena automáticamente usando `/usr/bin/time -v` cuando está disponible.
