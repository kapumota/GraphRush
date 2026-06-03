### Referencias de Fase 5

#### GBBS / Julienne

GBBS documenta Positive-Weight SSSP basado en la implementación de Julienne y bucketing work-efficient. Es la referencia más cercana a GraphRush porque ambos trabajan sobre memoria compartida y grafos en memoria.

#### tanshoo/delta-stepping

La implementación de tanshoo usa C++/MPI y memoria distribuida. GraphRush no adopta MPI en esta fase, pero usa el repositorio como referencia de validación, clasificación de aristas short/long y comparación contra Dijkstra.

#### PD3 HPEC 2024

PD3 propone Delta-Stepping con delta decreciente, balanceo de carga y reordenamiento del grafo por grado. GraphRush lo documenta como evolución futura, no como requisito de Fase 5.

#### Decisión final

```text
Fase 5: Delta-Stepping OpenMP sobre CSR estático, validado contra Dijkstra
Fase futura: pesos reales, delta decreciente, reordenamiento y balanceo avanzado
```
