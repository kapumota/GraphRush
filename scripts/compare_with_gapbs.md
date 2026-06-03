### Comparación con GAP Benchmark Suite

#### Objetivo

Definir el procedimiento para comparar GraphRush con GAPBS como baseline externo.

#### Supuesto

GAPBS se instala fuera del repositorio de GraphRush. No se versiona como submódulo para mantener el proyecto limpio.

#### Instalación sugerida

```bash
git clone https://github.com/sbeamer/gapbs external/gapbs
cd external/gapbs
make
```

#### Preparar datos

GraphRush usa `.grcsr`. GAPBS puede requerir su propio formato de entrada. Para una comparación rigurosa, conserva también el archivo original EdgeList/SNAP:

```text
data/raw/web-Google.txt
data/raw/roadNet-CA.txt
data/processed/web-google.grcsr
```

#### Ejecutar GraphRush

```bash
graphrush pagerank --graph data/processed/web-google.grcsr --iterations 20 --top-k 20 --json
graphrush bfs --graph data/processed/web-google.grcsr --source 0 --json
graphrush components --graph data/processed/web-google.grcsr --json
```

#### Ejecutar GAPBS

Los nombres exactos de binarios pueden variar según la compilación de GAPBS. La idea es mantener el mismo dataset, fuente e iteraciones cuando sea posible.

```bash
external/gapbs/pr -f data/raw/web-Google.txt -i20
external/gapbs/bfs -f data/raw/web-Google.txt -r1
external/gapbs/cc -f data/raw/web-Google.txt
```

#### Comparaciones mínimas

| Kernel | GraphRush | GAPBS | Comparación |
|---|---|---|---|
| BFS | nodos alcanzados, profundidad máxima | nodos alcanzados / validación | valores agregados |
| PageRank | top-k, delta L1 | scores / checksum | ranking y suma |
| Components | número de componentes, componente mayor | componentes | agregados |
| Dijkstra/SSSP | nodos alcanzados, distancia máxima | SSSP si aplica | agregados |

#### Reporte esperado

```text
reports/gapbs_comparison.md
reports/graph_results.json
reports/gapbs_results.txt
```

#### Criterio de aceptación

La comparación se considera válida si:

```text
se usa el mismo grafo original
se documenta si el grafo es dirigido o no dirigido
se documenta la fuente de BFS/SSSP
se documentan iteraciones de PageRank
los agregados coinciden o se explica la diferencia
```
