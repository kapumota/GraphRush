### GraphRush

#### Motor híbrido C++/Rust para análisis paralelo de grafos y ciberseguridad

GraphRush es un motor de análisis de grafos en memoria orientado a redes reales. El núcleo algorítmico se implementa en C++20 para controlar memoria, rendimiento y paralelismo. La CLI, validación de parámetros, configuración y reportes se implementan en Rust.

El proyecto está diseñado para evolucionar desde una Fase 0 mínima con frontera FFI segura hasta una versión funcional con carga de grafos reales, algoritmos secuenciales, algoritmos paralelos, benchmarks reproducibles, caminos mínimos y una aplicación profesional en ciberseguridad.

#### Objetivo profesional

GraphRush no se presenta como una base de datos de grafos completa. Se presenta como una herramienta de análisis estructural de redes para:

- ciberseguridad,
- análisis de caminos de ataque,
- detección de nodos críticos,
- análisis de dependencias,
- telecomunicaciones,
- logística,
- PageRank y análisis de redes web,
- procesamiento local de grafos medianos y grandes.

#### Lenguajes

| Capa | Lenguaje | Responsabilidad |
|---|---|---|
| Core algorítmico | C++20 | CSRGraph, carga eficiente, algoritmos y paralelismo |
| CLI y reportes | Rust | comandos, validación, configuración, reportes y salida profesional |
| Scripts auxiliares | Python | datasets, generación de grafos, gráficos de benchmark |
| Frontera FFI | cxx | puente tipado entre Rust y C++ |

#### Regla de arquitectura

Rust llama operaciones completas del core C++. C++ ejecuta los kernels críticos. No se cruza la frontera FFI por cada arista ni por cada vecino.

#### Fases del proyecto

| Fase | Nombre | Resultado |
|---:|---|---|
| 0 | Arquitectura híbrida C++/Rust y FFI | frontera técnica segura y skeleton funcional |
| 1 | GraphLoader + CSRGraph | carga de grafos reales |
| 2 | Algoritmos secuenciales | BFS, PageRank, componentes y Dijkstra |
| 3 | Paralelismo controlado | PageRank, BFS y componentes paralelos |
| 4 | Benchmark Engine | reportes reproducibles |
| 5 | Delta-Stepping SSSP | caminos mínimos paralelos |
| 6 | Security Pack | aplicación profesional en ciberseguridad |
| 7 | Release final | documentación, tests, benchmarks y demo completa |

#### Demo mínima de Fase 0

```bash
cd rust-cli
cargo run -- stats --graph ../data/small/example.edges
```

Salida esperada:

```text
[GraphRush] Grafo cargado correctamente.
[GraphRush] Nodos: 6
[GraphRush] Aristas: 8
[GraphRush] Grado máximo: 2
[GraphRush] Grado promedio: 1.3333
```

#### Comandos de apoyo

```bash
make build-rust
make demo
make build-cpp
make test-cpp
```

#### Estado de esta entrega

Esta entrega deja lista la Fase 0: arquitectura, documentación, estructura de repositorio, skeleton C++20, skeleton Rust y frontera FFI mediante `cxx`.
