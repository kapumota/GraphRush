### Fase 1 — Carga de grafos reales y representación CSR

#### Objetivo

Construir el motor base capaz de importar grafos reales y almacenarlos de forma compacta.

#### C++ implementa

```text
GraphLoader
EdgeListParser
CSRGraph
GraphStats
BinaryGraphWriter
BinaryGraphReader
```

#### Rust implementa

```text
graphrush import
graphrush stats
validación de rutas
salida JSON / tabla
```

#### Comandos esperados

```bash
graphrush import --input web-Google.txt --format snap --output web-google.gr
graphrush stats --graph web-google.gr
```

#### Métricas

```text
número de nodos
número de aristas
grado promedio
grado máximo
memoria aproximada
tiempo de carga
```

#### Cierre mediante Fase 1.1

La Fase 1.1 agrega las métricas y salidas faltantes para considerar cerrada la ingesta profesional de grafos.
