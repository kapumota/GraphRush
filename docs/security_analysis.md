### GraphRush - Análisis de seguridad

#### Alcance

GraphRush Security Pack analiza relaciones declaradas por el usuario en CSV. No recolecta credenciales, no ejecuta explotación, no reemplaza un SIEM y no reemplaza una plataforma EDR.

#### Valor profesional

```text
análisis de caminos de ataque
detección de identidades críticas
priorización de activos sensibles
detección de componentes conectadas
auditoría de relaciones de privilegio
```

#### Modelo de datos

```csv
source,target,relation,weight
attacker_machine,ip_10_0_0_5,reaches,1
ip_10_0_0_5,machine_22,connects_to,1
machine_22,user_15,logged_in_as,1
user_15,group_helpdesk,member_of,1
group_helpdesk,server_app,has_access,1
```

#### Comandos

```bash
graphrush security import --input data/security/security_edges.csv --output data/security/security.gr
graphrush security rank --graph data/security/security.gr --method pagerank --top 20
graphrush security path --graph data/security/security.gr --source attacker_machine --target database_server --algo bfs
graphrush security report --graph data/security/security.gr --output reports/security_report.md
```

#### Core bridge

La Fase 6.1 permite ejecutar análisis de seguridad sobre el core C++:

```bash
graphrush security core-rank --graph data/security/security.gr --output-dir reports/security_core --top 10 --threads 8
graphrush security core-path --graph data/security/security.gr --source attacker_machine --target database_server --output-dir reports/security_core --threads 8
graphrush security core-sssp --graph data/security/security.gr --source attacker_machine --output-dir reports/security_core --algo delta --delta 2 --threads 8
```
