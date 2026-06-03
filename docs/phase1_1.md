### Fase 1.1 - Métricas completas, salida JSON y validación reforzada

#### Objetivo

Cerrar profesionalmente la Fase 1 antes de iniciar la Fase 2. Esta fase agrega métricas faltantes, salida JSON y validación de rutas desde Rust.

#### Problema que resuelve

La Fase 1 ya permitía importar grafos reales, construir CSR, guardar binario y consultar estadísticas. Sin embargo, para cumplir completamente la especificación faltaban:

```text
salida JSON
memoria aproximada
tiempo de carga
validación explícita de rutas desde Rust
```

#### C++ implementa

```text
GraphStats con memory_bytes
GraphStats con load_time_ms
CsrGraph::memory_bytes()
CsrGraph::load_from_text()
CsrGraph::load_binary()
BinaryGraphWriter
BinaryGraphReader
validación interna CSR
```

#### Rust implementa

```text
graphrush import
graphrush stats
graphrush stats --json
graphrush validate
validación de existencia de archivo
salida humana
salida JSON
```

#### Comandos esperados

```bash
graphrush import --input web-Google.txt --format snap --output web-google.gr
graphrush stats --graph web-google.gr
graphrush stats --graph web-google.gr --json
graphrush validate --graph web-google.gr
```

#### Métricas

```text
número de nodos
número de aristas
grado promedio
grado máximo
memoria aproximada
tiempo de carga
estado de validación
```

#### Criterio de aceptación

La fase se considera cerrada cuando:

```text
un archivo EdgeList/SNAP/CSV puede importarse a .grcsr
el .grcsr puede leerse sin parsear el texto original
stats muestra métricas humanas
stats --json produce JSON válido
validate confirma la consistencia CSR
```

#### Resultado

Después de esta fase, GraphRush está listo para la Fase 2: algoritmos secuenciales sobre CSR.
