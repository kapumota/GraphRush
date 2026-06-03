### Demo de Fase 6.1 — Security Core Bridge

#### Paso 1 — Importar grafo simbólico

```bash
graphrush security import \
  --input data/security/security_edges.csv \
  --output data/security/security.gr
```

#### Paso 2 — Exportar a CSR numérico

```bash
graphrush security export-csr \
  --graph data/security/security.gr \
  --edge-list reports/security_core/security_numeric.edges \
  --mapping reports/security_core/security_mapping.json \
  --output-csr reports/security_core/security_numeric.grcsr
```

#### Paso 3 — PageRank en core C++

```bash
graphrush security core-rank \
  --graph data/security/security.gr \
  --output-dir reports/security_core \
  --top 10 \
  --iterations 20 \
  --threads 8
```

#### Paso 4 — BFS en core C++

```bash
graphrush security core-path \
  --graph data/security/security.gr \
  --source attacker_machine \
  --target database_server \
  --output-dir reports/security_core \
  --threads 8
```

#### Paso 5 — SSSP en core C++

```bash
graphrush security core-sssp \
  --graph data/security/security.gr \
  --source attacker_machine \
  --output-dir reports/security_core \
  --algo delta \
  --delta 2 \
  --threads 8
```
