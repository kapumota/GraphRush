### GraphRush

[![CI](https://github.com/kapumota/GraphRush/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/kapumota/GraphRush/actions/workflows/ci.yml)
[![validation](https://github.com/kapumota/GraphRush/actions/workflows/validation.yml/badge.svg?branch=main)](https://github.com/kapumota/GraphRush/actions/workflows/validation.yml)
[![release](https://img.shields.io/badge/release-v0.7--release--final-orange)](https://github.com/kapumota/GraphRush/releases)
[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
![C++](https://img.shields.io/badge/C%2B%2B-20-blue?logo=cplusplus)
![Rust](https://img.shields.io/badge/Rust-stable-orange?logo=rust)
![OpenMP](https://img.shields.io/badge/OpenMP-enabled-brightgreen)
![tests](https://img.shields.io/badge/tests-CTest%20%2B%20Cargo-brightgreen)
![benchmarks](https://img.shields.io/badge/benchmarks-reproducible-blue)
![security](https://img.shields.io/badge/security-pack-teal)

#### Motor híbrido C++20/Rust para procesamiento de grafos, benchmarking reproducible y análisis de ciberseguridad

GraphRush es un software de procesamiento de grafos reales en memoria, diseñado con una arquitectura híbrida: un núcleo algorítmico en C++20 y una CLI en Rust conectada mediante FFI segura con `cxx`.

El proyecto permite importar grafos reales, convertirlos a una representación CSR compacta, ejecutar algoritmos secuenciales y paralelos, evaluar rendimiento de forma reproducible y aplicar el motor a un caso profesional de ciberseguridad mediante análisis de caminos de ataque y nodos críticos.

GraphRush no es una demostración mínima de algoritmos. Es un sistema completo que integra:

```text
carga de grafos reales
formato binario .grcsr
representación CSR
BFS
Connected Components
PageRank
Dijkstra
Delta-Stepping
SSSP ponderado
paralelismo con OpenMP
Benchmark Engine
Security Pack
CLI Rust
FFI C++/Rust
CI con GitHub Actions
```

### Arquitectura general

#### Núcleo C++20

El núcleo C++20 implementa la representación interna del grafo y los algoritmos de alto rendimiento.

Componentes principales:

```text
CSRGraph
GraphLoader
EdgeListParser
BinaryGraphWriter
BinaryGraphReader
GraphStats
SequentialAlgorithms
ParallelAlgorithms
DeltaStepping
BucketQueue
AtomicDistanceArray
RelaxationEngine
```

La representación interna usa CSR para almacenar grafos dispersos de forma compacta. El formato `.grcsr` evita reparsing repetido de texto y permite cargar grafos procesados en ejecuciones posteriores.

#### CLI Rust

La CLI Rust actúa como capa de usuario, validación de argumentos y orquestación de comandos.

Comandos principales:

```text
import
stats
validate
bfs
components
pagerank
dijkstra
sssp
benchmark
security
```

La CLI no manipula directamente los arreglos internos del grafo. En su lugar, invoca funciones de alto nivel expuestas desde C++ mediante `cxx`.

#### Frontera FFI

La frontera C++/Rust expone operaciones de alto nivel, por ejemplo:

```text
load_graph
import_graph
graph_stats
run_bfs_report
run_components_report
run_pagerank_report
run_dijkstra_report
run_parallel_bfs_report
run_parallel_pagerank_report
run_sssp_report
write_sssp_distances_csv
```

No se exponen punteros crudos ni recorridos internos de aristas hacia Rust.

### Formatos soportados

#### Entrada textual

GraphRush puede importar grafos desde:

```text
EdgeList
SNAP
CSV
Weighted EdgeList
```

#### Formato binario

El formato interno serializado es:

```text
.grcsr
```

Este formato almacena la estructura CSR y, cuando corresponde, pesos de aristas para SSSP ponderado.

### Compilación

#### Requisitos

```text
C++20
CMake
g++
Rust stable
Cargo
Python 3.8+
OpenMP / libgomp
```

En Ubuntu:

```bash
sudo apt-get update
sudo apt-get install -y cmake g++ ninja-build python3 libgomp1
```

#### Compilar el core C++

```bash
cmake -S cpp-core -B build/cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build/cpp -j
ctest --test-dir build/cpp --output-on-failure
```

#### Compilar la CLI Rust

```bash
cd rust-cli
cargo build --release
cargo test
cargo clippy --all-targets -- -D warnings
cd ..
```

### Uso básico

#### Importar un grafo

```bash
cd rust-cli

cargo run -- import \
  --input ../data/small/example.edges \
  --format snap \
  --output ../data/small/example.grcsr \
  --directed \
  --deduplicate

cd ..
```

#### Ver estadísticas

```bash
cd rust-cli

cargo run -- stats \
  --graph ../data/small/example.grcsr

cargo run -- stats \
  --graph ../data/small/example.grcsr \
  --json

cd ..
```

#### Validar grafo

```bash
cd rust-cli

cargo run -- validate \
  --graph ../data/small/example.grcsr

cd ..
```

### Algoritmos secuenciales

#### BFS

```bash
cd rust-cli

cargo run -- bfs \
  --graph ../data/small/example.grcsr \
  --source 0

cd ..
```

#### Connected Components

```bash
cd rust-cli

cargo run -- components \
  --graph ../data/small/example.grcsr

cd ..
```

#### PageRank

```bash
cd rust-cli

cargo run -- pagerank \
  --graph ../data/small/example.grcsr \
  --iterations 20 \
  --top-k 5

cd ..
```

#### Dijkstra

```bash
cd rust-cli

cargo run -- dijkstra \
  --graph ../data/small/example.grcsr \
  --source 0

cd ..
```

### Algoritmos paralelos

GraphRush incorpora paralelismo con OpenMP en algoritmos seleccionados.

#### BFS paralelo por frontier

```bash
cd rust-cli

cargo run -- bfs \
  --graph ../data/small/example.grcsr \
  --source 0 \
  --threads 4

cd ..
```

#### PageRank paralelo

```bash
cd rust-cli

cargo run -- pagerank \
  --graph ../data/small/example.grcsr \
  --iterations 20 \
  --threads 4 \
  --top-k 5

cd ..
```

#### Componentes conexas en paralelo

```bash
cd rust-cli

cargo run -- components \
  --graph ../data/small/example.grcsr \
  --threads 4

cd ..
```

### SSSP y Delta-Stepping

GraphRush implementa Dijkstra como baseline y Delta-Stepping para SSSP ponderado.

#### Importar grafo ponderado

```bash
cd rust-cli

cargo run -- import \
  --input ../data/control/weighted_sssp.edges \
  --format snap \
  --output ../data/control/weighted_sssp.grcsr \
  --directed \
  --deduplicate \
  --weighted

cd ..
```

#### Dijkstra ponderado

```bash
cd rust-cli

cargo run -- sssp \
  --graph ../data/control/weighted_sssp.grcsr \
  --source 0 \
  --algo dijkstra

cd ..
```

#### Delta-Stepping ponderado

```bash
cd rust-cli

cargo run -- sssp \
  --graph ../data/control/weighted_sssp.grcsr \
  --source 0 \
  --algo delta \
  --delta 2 \
  --threads 4 \
  --compare \
  --output-csv ../reports/weighted_delta.csv

cd ..
```

### Benchmark Engine

GraphRush incluye un motor de evaluación reproducible que genera reportes en CSV, JSON y Markdown.

```bash
cd rust-cli

cargo run -- benchmark \
  --graph ../data/small/example.grcsr \
  --algos bfs,pagerank,components \
  --threads 1,2,4 \
  --output ../reports/benchmark_demo \
  --source 0 \
  --iterations 20 \
  --top-k 5

cd ..
```

Métricas principales:

```text
tiempo total
throughput
aristas procesadas por segundo
GTEPS
speedup
eficiencia paralela
memoria pico
```

### Security Pack

GraphRush incluye un módulo de aplicación profesional en ciberseguridad.

El Security Pack modela relaciones como:

```text
usuario -> grupo
grupo -> permiso
usuario -> máquina
máquina -> servicio
servicio -> base de datos
IP -> máquina
cuenta -> activo crítico
```

Ejemplo conceptual:

```text
attacker_machine -> ip_10_0_0_5 -> machine_22 -> user_15 -> group_admin -> server_db -> critical_asset
```

#### Importar grafo de seguridad

```bash
cd rust-cli

cargo run -- security import \
  --input ../data/security/security_edges.csv \
  --output ../data/security/security.gr

cd ..
```

#### Estadísticas

```bash
cd rust-cli

cargo run -- security stats \
  --graph ../data/security/security.gr

cd ..
```

#### Ranking de nodos críticos

```bash
cd rust-cli

cargo run -- security rank \
  --graph ../data/security/security.gr \
  --method pagerank \
  --top 10

cd ..
```

#### Camino de ataque

```bash
cd rust-cli

cargo run -- security path \
  --graph ../data/security/security.gr \
  --source attacker_machine \
  --target critical_asset \
  --algo bfs

cd ..
```

#### Reporte

```bash
cd rust-cli

cargo run -- security report \
  --graph ../data/security/security.gr \
  --output ../reports/security_report.md

cd ..
```

#### Puente al core C++

El Security Pack puede convertir el grafo simbólico a un grafo numérico y ejecutar PageRank, BFS o SSSP usando el core C++ de GraphRush.

```bash
cd rust-cli

cargo run -- security core-rank \
  --graph ../data/security/security.gr \
  --output-dir ../reports/security_core \
  --top 10 \
  --iterations 20 \
  --threads 4

cd ..
```

### Estructura del repositorio

```text
README.md
LICENSE
CHANGELOG.md
Makefile

cpp-core/
rust-cli/
scripts/
data/

docs/
  architecture.md
  ffi_boundary.md
  evaluation.md
  benchmarks.md
  security_analysis.md
  release_checklist.md
  screenshots/
  archive/

reports/
  final_report.md
```

### Documentación técnica

```text
docs/architecture.md
docs/ffi_boundary.md
docs/evaluation.md
docs/benchmarks.md
docs/security_analysis.md
reports/final_report.md
```

La documentación histórica por fases se conserva en:

```text
docs/archive/phases/
docs/archive/references/
docs/archive/demos/
```


### Validación reproducible del software

GraphRush se valida mediante un flujo reproducible conectado a GitHub Actions. El workflow `.github/workflows/validation.yml` ejecuta el script principal:

```bash
make validate
```

Este proceso comprueba estructura del repositorio, compilación del core C++20, pruebas con CTest, compilación de la CLI Rust, pruebas con Cargo, análisis con Clippy, sintaxis de scripts Python, importación de grafos, ejecución de algoritmos y una demo mínima del Security Pack.

La validación local también puede ejecutarse con:

```bash
bash scripts/validate.sh
```

El objetivo es que los badges del README no sean solo decorativos. El estado de validación depende de comandos ejecutables y de un workflow verificable.

### Verificación local completa

```bash
cmake -S cpp-core -B build/cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build/cpp -j
ctest --test-dir build/cpp --output-on-failure
```

```bash
cd rust-cli
cargo build --release
cargo test
cargo clippy --all-targets -- -D warnings
cd ..
```

```bash
python3 -m py_compile scripts/security_pack.py
python3 -m py_compile scripts/benchmark_engine.py
python3 -m py_compile scripts/phase3_parallel_benchmark.py
```

### Estado del proyecto

GraphRush se encuentra en versión final de release académico-profesional. Incluye carga de grafos reales, algoritmos secuenciales, paralelismo, Benchmark Engine, SSSP ponderado con Delta-Stepping, Security Pack, documentación técnica y CI con GitHub Actions.

### Licencia

Ver `LICENSE`.

### Referencias internas

```text
docs/architecture.md
docs/ffi_boundary.md
docs/evaluation.md
docs/benchmarks.md
docs/security_analysis.md
reports/final_report.md
```
