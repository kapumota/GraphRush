### Fase 4.1 — Rutas robustas del Benchmark Engine y demo completa

#### Objetivo

Robustecer el comando `graphrush benchmark` para que no dependa estrictamente de ejecutarse desde una ruta frágil, y agregar una demostración completa usando `data/small/example.grcsr`.

#### Problema que resuelve

La Fase 4 ya incluía el Benchmark Engine, pero el comando Rust llamaba al script mediante una ruta relativa fija:

```text
scripts/benchmark_engine.py
```

Eso funciona desde la raíz del repositorio, pero puede fallar si el usuario ejecuta el binario desde `rust-cli/` u otra ubicación.

#### Solución aplicada

El comando `benchmark` ahora intenta resolver el script de dos formas:

```text
1. scripts/benchmark_engine.py desde el directorio actual
2. ../scripts/benchmark_engine.py relativo a CARGO_MANIFEST_DIR
```

Además, el runner ya no pasa `--binary graphrush` de forma rígida. Ahora usa:

```text
std::env::current_exe()
```

para pasar al Benchmark Engine la ruta real del binario que está ejecutándose.

#### Comando oficial

```bash
graphrush benchmark \
  --graph data/small/example.grcsr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --output reports/
```

#### Demo completa desde cero

```bash
cd graphrush

cd rust-cli
cargo build --release
cd ..

./rust-cli/target/release/graphrush-cli import \
  --input data/small/example.edges \
  --format snap \
  --output data/small/example.grcsr \
  --directed \
  --deduplicate

./rust-cli/target/release/graphrush-cli benchmark \
  --graph data/small/example.grcsr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8,16 \
  --output reports/
```

#### Salidas esperadas

```text
reports/benchmark.csv
reports/benchmark.json
reports/benchmark_results.md
reports/perf_summary.md
```

#### Criterio de aceptación

La fase se considera completa si el comando `benchmark` puede ubicar `benchmark_engine.py` desde la raíz del repositorio y desde el flujo normal de compilación de Rust, y si la demo con `data/small/example.grcsr` genera todos los reportes esperados.
