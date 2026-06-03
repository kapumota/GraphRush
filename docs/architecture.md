### Arquitectura de GraphRush

#### Vista general

```text
┌──────────────────────────────────────────────┐
│              Rust CLI                         │
│  import, stats, validate, reportes            │
└───────────────────────┬──────────────────────┘
                        │ cxx
┌───────────────────────▼──────────────────────┐
│              FFI Boundary                      │
│  tipos opacos + funciones de alto nivel        │
└───────────────────────┬──────────────────────┘
                        │
┌───────────────────────▼──────────────────────┐
│              C++20 Core                        │
│  CSRGraph, loaders, binario, stats             │
└──────────────────────────────────────────────┘
```

#### Core C++20

```text
CSRGraph
GraphLoader
EdgeListParser
BinaryGraphWriter
BinaryGraphReader
GraphStats
```

#### CLI Rust

```text
graphrush import
graphrush stats
graphrush stats --json
graphrush validate
```

#### Representación interna

GraphRush usa CSR:

```text
offsets: tamaño n + 1
neighbors: tamaño m
```

#### Formato binario

GraphRush usa un formato propio `.grcsr` con:

```text
magic header
versión
número de nodos
número de aristas
offsets
neighbors
```
