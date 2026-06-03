### Graph500, GAP y ChunkGraph en GraphRush

#### Graph500

Graph500 estandariza benchmarks para grafos de gran escala, especialmente BFS y SSSP. Su métrica tradicional es TEPS/GTEPS, que mide aristas recorridas por segundo.

GraphRush adopta `GTEPS` como métrica adicional en Fase 4.

#### GAP Benchmark Suite

GAPBS estandariza kernels, datasets, metodología e implementaciones baseline optimizadas. GraphRush adopta su enfoque de evaluación repetible y comparación por número de hilos.

#### ChunkGraph

ChunkGraph es una referencia moderna para procesamiento eficiente de grafos grandes con representación por chunks. GraphRush todavía usa CSR estático, pero deja el diseño preparado para comparar en el futuro con layouts por chunks.

#### Decisión final

```text
Fase 4: CSR estático + GTEPS + metodología GAP + reportes reproducibles
Fase futura: comparar layouts CSR, chunked CSR y GraphCSR si el proyecto crece
```
