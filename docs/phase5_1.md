### Fase 5.1 — Weighted SSSP

#### Objetivo

Extender GraphRush para soportar caminos mínimos con pesos reales por arista.

#### Alcance

```text
weighted EdgeList
vector weights
formato .grcsr con pesos
Dijkstra ponderado
Delta-Stepping ponderado
validación contra Dijkstra ponderado
```

#### Formato de entrada ponderado

```text
source target weight
```

Ejemplo:

```text
0 1 2.5
0 2 1.0
2 1 0.5
1 3 2.0
2 3 5.0
```

#### Importación ponderada

```bash
graphrush import \
  --input data/control/weighted_sssp.edges \
  --format snap \
  --output data/control/weighted_sssp.grcsr \
  --directed \
  --deduplicate \
  --weighted
```

#### Dijkstra ponderado

```bash
graphrush sssp \
  --graph data/control/weighted_sssp.grcsr \
  --source 0 \
  --algo dijkstra \
  --output-csv reports/weighted_dijkstra.csv
```

#### Delta-Stepping ponderado

```bash
graphrush sssp \
  --graph data/control/weighted_sssp.grcsr \
  --source 0 \
  --algo delta \
  --delta 2 \
  --threads 8 \
  --compare \
  --output-csv reports/weighted_delta.csv
```

#### Cambios técnicos

```text
CsrGraph agrega vector<double> weights
BinaryGraphIO agrega versión 2 con flag weighted
EdgeListParser acepta tercera columna de peso
Dijkstra usa graph.edge_weight(idx)
Delta-Stepping clasifica light/heavy con graph.edge_weight(idx)
```

#### Compatibilidad

El lector binario conserva compatibilidad con `.grcsr` versión 1 sin pesos. Si no existen pesos, `edge_weight(idx)` devuelve `1.0`.

#### Limitación

No se permiten pesos negativos porque Dijkstra y Delta-Stepping no son válidos para pesos negativos.
