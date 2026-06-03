### Arquitectura de GraphRush

#### Vista general

```text
┌──────────────────────────────────────────────┐
│              Rust CLI                         │
│  comandos, validación, configuración, reportes│
└───────────────────────┬──────────────────────┘
                        │ cxx
┌───────────────────────▼──────────────────────┐
│              FFI Boundary                      │
│  tipos opacos + funciones de alto nivel        │
└───────────────────────┬──────────────────────┘
                        │
┌───────────────────────▼──────────────────────┐
│              C++20 Core                        │
│  CSRGraph, loaders, algoritmos y paralelismo   │
└──────────────────────────────────────────────┘
```

#### Core C++20

El core contiene estructuras y algoritmos de alto rendimiento:

```text
CSRGraph
GraphLoader
BFS
PageRank
ConnectedComponents
Dijkstra
DeltaStepping
```

#### CLI Rust

La CLI contiene la experiencia de usuario:

```text
graphrush import
graphrush stats
graphrush bfs
graphrush pagerank
graphrush components
graphrush sssp
graphrush benchmark
graphrush security
```

#### Frontera FFI

La frontera se mantiene pequeña. Rust no conoce el layout interno de CSR. C++ no delega loops críticos a Rust.

#### Decisión inicial

La Fase 0 usa `cxx` como puente. Si en fases posteriores se requiere un build CMake unificado, se puede evaluar Corrosion.
