### Demo de Fase 6 - Security Pack

#### Paso 1 - Importar grafo de seguridad

```bash
graphrush security import \
  --input data/security/security_edges.csv \
  --output data/security/security.gr
```

#### Paso 2 - Estadísticas

```bash
graphrush security stats \
  --graph data/security/security.gr
```

#### Paso 3 - Ranking de nodos críticos

```bash
graphrush security rank \
  --graph data/security/security.gr \
  --method pagerank \
  --top 10
```

#### Paso 4 - Camino de ataque

```bash
graphrush security path \
  --graph data/security/security.gr \
  --source attacker_machine \
  --target database_server \
  --algo bfs
```

#### Paso 5 - Componentes

```bash
graphrush security components \
  --graph data/security/security.gr \
  --min-size 2
```

#### Paso 6 - Reporte

```bash
graphrush security report \
  --graph data/security/security.gr \
  --output reports/security_report.md
```

#### Salidas

```text
reports/security_report.md
reports/critical_nodes.csv
reports/attack_paths.json
reports/components.json
reports/security_summary.md
```
