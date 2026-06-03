### Diseño de GraphRush

#### Resumen

GraphRush es un motor híbrido de análisis de grafos. La capa de alto rendimiento se implementa en C++20 y la capa de producto se implementa en Rust. El diseño evita exponer detalles internos del grafo a Rust, la CLI solo invoca operaciones completas sobre tipos opacos.

#### Principios de diseño

#### Separación estricta de responsabilidades

C++20 controla:

- representación CSR,
- carga eficiente,
- algoritmos,
- paralelismo,
- medición de bajo nivel.

Rust controla:

- CLI,
- validación de argumentos,
- configuración,
- reportes,
- salida para usuario,
- automatización de flujos.

#### Frontera FFI de alto nivel

La frontera FFI debe exponer únicamente:

- tipos opacos,
- funciones de alto nivel,
- resultados simples,
- métricas agregadas.

No debe exponer:

- punteros crudos,
- iteradores C++,
- detalles internos de CSR,
- loops sobre vecinos,
- memoria temporal,
- estructuras STL por valor.

#### Representación inicial del grafo

La Fase 0 usa una representación CSR compacta:

```text
offsets[u] ... offsets[u + 1] define el rango de vecinos de u
neighbors contiene todos los vecinos de forma contigua
```

#### Evolución esperada

La arquitectura permite evolucionar hacia:

- formato binario `.gr`,
- PageRank paralelo,
- BFS por frontier,
- Connected Components,
- Dijkstra,
- Delta-Stepping,
- Security Pack,
- API futura HTTP/gRPC.
