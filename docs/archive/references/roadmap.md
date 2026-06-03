### Roadmap general de GraphRush

#### Fase 0 - Arquitectura híbrida C++/Rust y FFI

Define la frontera entre Rust y C++ usando `cxx`, tipos opacos y funciones de alto nivel.

#### Fase 1 - GraphLoader y CSRGraph

Construye un cargador para grafos reales en formato EdgeList, SNAP o CSV y los almacena en CSR.

#### Fase 2 - Algoritmos secuenciales

Implementa BFS, PageRank, Connected Components y Dijkstra como baseline correcto.

#### Fase 3 - Paralelismo controlado

Agrega PageRank paralelo, BFS paralelo por frontier y Connected Components paralelo.

#### Fase 4 - Benchmark Engine

Agrega mediciones reproducibles: tiempo, throughput, speedup, eficiencia, memoria y reportes.

#### Fase 5 - Delta-Stepping SSSP

Implementa caminos mínimos paralelos mediante buckets y relajaciones concurrentes.

#### Fase 6 - Security Pack

Agrega aplicación profesional de ciberseguridad: caminos de ataque, nodos críticos, componentes sospechosas y reportes.

#### Fase 7 - Release final

Entrega documentación, tests, benchmarks, reportes, demo reproducible, licencia y changelog.
