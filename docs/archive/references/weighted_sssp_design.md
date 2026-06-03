### Diseño de Weighted SSSP

#### Vector de pesos

GraphRush almacena pesos en:

```text
std::vector<double> weights
```

El índice de `weights[i]` corresponde al vecino `neighbors[i]`.

#### API

```text
bool has_weights()
double edge_weight(edge_index)
const std::vector<double>& weights()
```

#### Formato binario

La versión 2 del formato `.grcsr` agrega:

```text
version = 2
flags = weighted
offsets
neighbors
weights
```

La versión 1 continúa funcionando para grafos no ponderados.

#### Dijkstra ponderado

La relajación cambia de:

```text
candidate = distance + 1.0
```

a:

```text
candidate = distance + graph.edge_weight(edge_index)
```

#### Delta-Stepping ponderado

La clasificación se hace con:

```text
light edge: weight <= delta
heavy edge: weight > delta
```

#### Validación

`test_weighted_graph_io.cpp` compara Dijkstra ponderado contra Delta-Stepping ponderado en un grafo pequeño con pesos conocidos.
