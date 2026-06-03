### GraphRush - Evaluación

#### Objetivo

Definir cómo validar GraphRush como software funcional y reproducible.

#### Validación del core C++

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

#### Validación de Rust

```bash
cd rust-cli
cargo build --release
cargo test
cargo clippy --all-targets -- -D warnings
```

#### Validación de Benchmark Engine

```bash
graphrush benchmark \
  --graph data/small/example.grcsr \
  --algos bfs,pagerank,components \
  --threads 1,2,4,8 \
  --output reports/
```

#### Validación de Security Pack

```bash
graphrush security report \
  --graph data/security/security.gr \
  --output reports/security_report.md
```

#### Criterios de aceptación

```text
compilación C++ correcta
tests C++ correctos
compilación Rust correcta
clippy sin warnings bloqueantes
benchmark reproducible
Security Pack genera reportes
no se versionan builds ni artefactos generados
```
