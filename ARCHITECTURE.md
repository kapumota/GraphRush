### GraphRush — Arquitectura final

#### Visión general

GraphRush es un motor híbrido C++20/Rust/Python para procesamiento de grafos, benchmarking reproducible y análisis estructural de ciberseguridad.

#### Capas

```text
Rust CLI
  ├── comandos de usuario
  ├── validación de rutas
  ├── dispatch hacia core C++
  └── runners Python para benchmarking y security pack

C++20 Core
  ├── CSRGraph
  ├── GraphLoader
  ├── BinaryGraphIO
  ├── algoritmos secuenciales
  ├── algoritmos paralelos OpenMP
  ├── Delta-Stepping
  └── Weighted SSSP

Python Runners
  ├── Benchmark Engine
  ├── Security Pack
  └── scripts auxiliares reproducibles
```

#### Flujo de datos principal

```text
EdgeList / SNAP / CSV
→ GraphLoader
→ CSRGraph
→ .grcsr
→ algoritmos
→ reportes
```

#### Flujo de ciberseguridad

```text
security_edges.csv
→ security.gr simbólico
→ security_numeric.edges
→ security_mapping.json
→ security_numeric.grcsr
→ PageRank / BFS / SSSP en core C++
→ reportes de seguridad
```

#### Decisión técnica

GraphRush mantiene el core algorítmico en C++20 para rendimiento y la CLI en Rust para ergonomía y seguridad de interfaz. Python se usa únicamente como capa de orquestación reproducible para benchmarking y análisis de seguridad.
