### Fase 3.2 — Medición real de memoria pico

#### Objetivo

Cerrar literalmente la métrica de memoria pico solicitada en la Fase 3 mediante integración con `/usr/bin/time -v`.

#### Problema que resuelve

La Fase 3.1 generaba la columna `memory_peak_kb`, pero todavía no la llenaba automáticamente. Esta fase modifica `scripts/phase3_parallel_benchmark.py` para ejecutar cada comando mediante:

```bash
/usr/bin/time -v
```

y extraer:

```text
Maximum resident set size (kbytes)
```

#### Comando

```bash
python scripts/phase3_parallel_benchmark.py \
  --binary ./target/release/graphrush \
  --graph data/web-google.gr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --source 42 \
  --iterations 30 \
  --output reports/phase3 \
  --measure-memory
```

#### Desactivar medición de memoria

Si el sistema no tiene `/usr/bin/time`, se puede ejecutar:

```bash
python scripts/phase3_parallel_benchmark.py \
  --binary ./target/release/graphrush \
  --graph data/web-google.gr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --no-measure-memory
```

#### Salidas

```text
reports/phase3/parallel_benchmark.csv
reports/phase3/parallel_benchmark.json
reports/phase3/parallel_benchmark.md
```

#### Métrica agregada

```text
memory_peak_kb
```

#### Criterio de aceptación

La fase se considera completa cuando `parallel_benchmark.csv`, `parallel_benchmark.json` y `parallel_benchmark.md` incluyen valores reales en la columna `memory_peak_kb` cuando `/usr/bin/time -v` está disponible.
