### Fase 5 — Caminos mínimos paralelos con Delta-Stepping

#### Objetivo

Implementar el componente avanzado del motor: shortest path paralelo.

#### C++ implementa

```text
DeltaStepping
BucketQueue
AtomicDistanceArray
RelaxationEngine
Dijkstra baseline
```

#### Rust implementa

```text
graphrush sssp
parámetro --delta
comparación contra Dijkstra
exportación de distancias
```

#### Comandos

```bash
graphrush sssp \
  --graph road-ca.gr \
  --source 0 \
  --algo dijkstra

graphrush sssp \
  --graph road-ca.gr \
  --source 0 \
  --algo delta \
  --delta 4 \
  --threads 8 \
  --compare
```

#### Exportación CSV

```bash
graphrush sssp \
  --graph road-ca.gr \
  --source 0 \
  --algo delta \
  --delta 4 \
  --threads 8 \
  --compare \
  --output-csv reports/sssp_distances.csv
```

#### Criterio de éxito

Delta-Stepping debe producir las mismas distancias que Dijkstra en grafos de prueba, y debe mostrar mejora cuando el grafo y el hardware permitan paralelismo real.

#### Decisión de implementación

La Fase 5 implementa Delta-Stepping en memoria compartida con OpenMP y CSR estático. La primera versión usa pesos unitarios por arista, coherente con el Dijkstra baseline existente. Esto permite validar correctamente la ruta algorítmica antes de introducir pesos reales en una fase posterior.

#### Referencias técnicas

```text
GBBS / Julienne: referencia principal de diseño para memoria compartida y bucketing
tanshoo/delta-stepping: referencia para clasificación short/long y validación con Dijkstra
PD3 HPEC 2024: referencia futura para delta decreciente y balanceo avanzado
```

#### Limitación consciente

El grafo actual no almacena pesos por arista. Por eso Delta-Stepping usa peso unitario. La extensión a grafos ponderados requiere ampliar el formato `.grcsr` con un vector `weights`.
