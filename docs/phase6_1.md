### Fase 6.1 — Security Graph Core Bridge

#### Objetivo

Mapear el grafo de seguridad simbólico a `.grcsr` numérico y ejecutar PageRank, BFS y SSSP directamente en el core C++ de alto rendimiento.

#### Problema que resuelve

La Fase 6 permitía analizar un grafo de seguridad con nombres simbólicos mediante `scripts/security_pack.py`. Eso es útil para reportes, pero no usa directamente el core C++ de GraphRush.

La Fase 6.1 agrega un puente:

```text
security.gr simbólico
-> security_numeric.edges
-> security_mapping.json
-> security_numeric.grcsr
-> core C++ PageRank/BFS/SSSP
-> reportes con nombres simbólicos
```

#### Nuevos comandos

```bash
graphrush security export-csr \
  --graph data/security/security.gr \
  --edge-list data/security/security_numeric.edges \
  --mapping data/security/security_mapping.json \
  --output-csr data/security/security_numeric.grcsr
```

```bash
graphrush security core-rank \
  --graph data/security/security.gr \
  --output-dir reports/security_core \
  --top 20 \
  --iterations 20 \
  --threads 8
```

```bash
graphrush security core-path \
  --graph data/security/security.gr \
  --source attacker_machine \
  --target database_server \
  --output-dir reports/security_core \
  --threads 8
```

```bash
graphrush security core-sssp \
  --graph data/security/security.gr \
  --source attacker_machine \
  --output-dir reports/security_core \
  --algo delta \
  --delta 2 \
  --threads 8
```

#### Archivos generados

```text
reports/security_core/security_numeric.edges
reports/security_core/security_mapping.json
reports/security_core/security_numeric.grcsr
reports/security_core/core_critical_nodes.csv
reports/security_core/core_rank.md
reports/security_core/core_rank.json
reports/security_core/core_attack_path.json
reports/security_core/core_bfs_distances.csv
reports/security_core/core_sssp_distances.csv
reports/security_core/core_sssp_distances.json
```

#### Decisión técnica

El Security Pack conserva nombres simbólicos para reportes. El core C++ opera con IDs numéricos. El archivo `security_mapping.json` mantiene la trazabilidad entre ambas representaciones.

#### Nota sobre rutas

`core-path` ejecuta BFS en el core C++ para obtener distancias y reconstruye el camino simbólico usando el grafo de seguridad original, porque el core actual exporta distancias pero no padres de BFS. Esta decisión mantiene el uso del core para cómputo pesado sin perder interpretabilidad.
