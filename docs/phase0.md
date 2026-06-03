### Fase 0 - Arquitectura híbrida C++/Rust y FFI

#### Objetivo

Definir la arquitectura base de GraphRush como un motor híbrido. El núcleo de alto rendimiento se implementa en C++20, mientras que Rust se usa para la CLI, validación, configuración y generación de reportes.

#### Decisión técnica

GraphRush utilizará la crate `cxx` para conectar Rust y C++. La frontera FFI se define con `#[cxx::bridge]`, exponiendo tipos opacos y funciones de alto nivel.

#### Responsabilidades

| Capa | Lenguaje | Responsabilidad |
|---|---|---|
| Core de grafos | C++20 | CSRGraph, carga eficiente y algoritmos |
| CLI | Rust | comandos, validación, configuración y reportes |
| FFI | cxx | comunicación tipada entre Rust y C++ |
| Scripts | Python | datasets, gráficos y automatización experimental |

#### Regla de diseño

Rust debe invocar operaciones completas sobre el grafo. C++ debe ejecutar los kernels algorítmicos completos. La frontera FFI no debe cruzarse por cada arista o por cada vecino.

#### Interfaz FFI mínima

La interfaz inicial expone:

```text
CsrGraph como tipo opaco
load_graph(path)
node_count(graph)
edge_count(graph)
max_degree(graph)
average_degree(graph)
```

#### Criterio de aceptación

La fase se considera completa cuando Rust puede invocar una función C++ que carga un grafo pequeño, construye una representación CSR y devuelve estadísticas básicas mediante una interfaz FFI tipada.

#### Comando de validación

```bash
cd rust-cli
cargo run -- stats --graph ../data/small/example.edges
```

#### Salida esperada

```text
[GraphRush] Grafo cargado correctamente.
[GraphRush] Nodos: 6
[GraphRush] Aristas: 8
[GraphRush] Grado máximo: 2
[GraphRush] Grado promedio: 1.3333
```

#### Convenciones de estilo

- Los títulos en documentación usan `###`.
- Los subtítulos en documentación usan `####`.
- Los comentarios del código están en español.
- Las cadenas visibles para el usuario están en español.
- Las firmas, funciones, structs y módulos usan nombres en inglés.
