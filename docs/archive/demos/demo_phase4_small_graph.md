### Demo completa de Fase 4 con grafo pequeño

#### Objetivo

Ejecutar el Benchmark Engine de GraphRush usando el grafo de control `data/small/example.edges`.

#### Paso 1 - Compilar CLI

```bash
cd graphrush/rust-cli
cargo build --release
cd ..
```

#### Paso 2 - Importar grafo de ejemplo

```bash
./rust-cli/target/release/graphrush-cli import \
  --input data/small/example.edges \
  --format snap \
  --output data/small/example.grcsr \
  --directed \
  --deduplicate
```

#### Paso 3 - Ejecutar benchmark

```bash
./rust-cli/target/release/graphrush-cli benchmark \
  --graph data/small/example.grcsr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --output reports/
```

#### Paso 4 - Revisar salidas

```bash
ls reports/
cat reports/benchmark_results.md
```

#### Archivos esperados

```text
reports/benchmark.csv
reports/benchmark.json
reports/benchmark_results.md
reports/perf_summary.md
```

#### Nota

El grafo pequeño sirve para verificar el flujo completo. Para evaluación de rendimiento real se deben usar grafos como `web-Google`, `roadNet-CA` o datasets SNAP medianos.
