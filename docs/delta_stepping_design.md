### Diseño de Delta-Stepping en GraphRush

#### Componentes

#### AtomicDistanceArray

Arreglo de distancias con relajación atómica mediante compare-and-swap. En Fase 5 usa codificación entera escalada para conservar una API de distancias `double` con pesos unitarios.

#### BucketQueue

Estructura de buckets parametrizada por `delta`.

```text
bucket_index = floor(distance / delta)
```

#### RelaxationEngine

Separa la relajación de aristas ligeras y pesadas:

```text
light edges: peso <= delta
heavy edges: peso > delta
```

En Fase 5 todas las aristas tienen peso unitario.

#### DeltaStepping

Orquesta el algoritmo:

```text
1. inicializar distancia[source] = 0
2. insertar source en bucket 0
3. procesar buckets en orden creciente
4. relajar aristas ligeras hasta estabilizar bucket
5. relajar aristas pesadas
6. repetir hasta vaciar buckets
```

#### Validación

`test_delta_stepping.cpp` compara todas las distancias contra Dijkstra secuencial.
