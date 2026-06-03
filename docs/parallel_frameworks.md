### Frameworks de paralelismo considerados

#### Ligra y Ligra+

Ligra es una referencia directa para GraphRush porque propone primitivas de frontera como `edgeMap` y `vertexMap`. Sus aplicaciones incluyen BFS, PageRank, componentes conexas, Bellman-Ford y K-Core.

GraphRush no copia Ligra. Usa sus ideas para orientar BFS por frontier y futuras abstracciones de recorrido.

#### GAP Benchmark Suite

GAPBS es la referencia principal de metodología y comparación de rendimiento. Usa OpenMP y provee kernels optimizados para evaluación en memoria compartida.

GraphRush adopta OpenMP en Fase 3 para mantener una ruta de comparación razonable con GAPBS.

#### NWGraph + HPX

NWGraph integrado con HPX representa una ruta futura para ejecución distribuida. No se incorpora en Fase 3 porque GraphRush todavía trabaja sobre una sola máquina y CSR estático.

#### DCSR

DCSR es relevante para grafos dinámicos y streaming graph processing. GraphRush no lo necesita todavía porque Fase 1 a Fase 5 se basan en CSR estático.

#### Decisión final

```text
Fase 3: OpenMP + C++20 sobre CSR estático
Fase futura: evaluar HPX/NWGraph si se busca distribución
Fase futura: evaluar DCSR si se agregan updates dinámicos
```
