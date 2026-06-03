### Baseline GAP Benchmark Suite

#### Objetivo

Documentar cómo GraphRush usará GAP Benchmark Suite como referencia externa de metodología, correctitud y rendimiento.

#### Rol de GAPBS en GraphRush

GAPBS no se copia dentro del core de GraphRush. Se usa como baseline externo para comparar resultados y metodología de evaluación.

#### Kernels relevantes

```text
BFS
PageRank
Connected Components
SSSP
Betweenness Centrality
Triangle Counting
```

#### Uso en Fase 2

En Fase 2 y 2.1, GAPBS sirve para:

```text
validar criterios de evaluación
definir nombres de kernels
comparar salidas agregadas
preparar benchmarks reproducibles
```

#### Uso en Fase 3 y Fase 4

En fases posteriores, GAPBS se puede usar para comparar:

```text
tiempo de ejecución
throughput
speedup
uso de memoria
comportamiento con grafos grandes
```

#### Decisión de diseño

GraphRush implementa baselines secuenciales propios para mantener el código explicable. Luego, GAPBS se ejecuta como herramienta externa en scripts de comparación.

#### Comandos esperados de comparación

```bash
bash scripts/run_gapbs_reference.sh data/small/example.grcsr reports/gapbs/
```

#### Limitación

GAPBS utiliza sus propios formatos y herramientas de conversión. Por eso la comparación automática completa puede requerir adaptar datasets a formatos compatibles con GAPBS.
