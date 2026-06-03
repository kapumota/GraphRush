### Fase 6 — Aplicación profesional en ciberseguridad

#### Objetivo

Demostrar que GraphRush no es solo académico, sino una herramienta útil para analizar relaciones de seguridad en una red empresarial.

#### Nombre del módulo

GraphRush Security Pack.

#### Descripción

Módulo de análisis de grafos para modelar entidades de ciberseguridad y detectar caminos críticos, nodos privilegiados, componentes sospechosas y rutas potenciales de ataque.

#### Inspiración profesional

El modelo se inspira en herramientas de análisis de caminos de ataque como BloodHound, donde identidades, grupos, permisos y activos se modelan como grafos dirigidos. GraphRush no reemplaza a BloodHound, SIEM, EDR ni herramientas de auditoría de Active Directory; su función es demostrar análisis estructural sobre datos CSV reproducibles.

#### Modelo de grafo de seguridad

```text
usuario -> grupo
grupo -> permiso
usuario -> máquina
máquina -> servicio
servicio -> base de datos
IP -> dominio
proceso -> archivo
proceso -> conexión
cuenta -> recurso crítico
```

#### Datos de entrada

```csv
source,target,relation,weight
user_15,group_admin,member_of,1
group_admin,server_db,has_access,1
machine_22,user_15,logged_in_as,1
ip_10.0.0.5,machine_22,connects_to,1
```

#### Comandos profesionales

```bash
graphrush security import \
  --input data/security/security_edges.csv \
  --output data/security/security.gr

graphrush security stats \
  --graph data/security/security.gr

graphrush security rank \
  --graph data/security/security.gr \
  --method pagerank \
  --top 50

graphrush security path \
  --graph data/security/security.gr \
  --source attacker_machine \
  --target database_server \
  --algo bfs

graphrush security components \
  --graph data/security/security.gr \
  --min-size 10

graphrush security report \
  --graph data/security/security.gr \
  --output reports/security_report.md
```

#### Preguntas profesionales que responde

```text
¿Qué usuarios tienen más influencia en la red?
¿Cuál es el camino más corto desde una máquina comprometida hasta un servidor crítico?
¿Qué grupos concentran demasiados permisos?
¿Qué máquinas están conectadas a componentes sensibles?
¿Qué componentes quedan aisladas o sospechosamente densas?
¿Qué nodos deberían auditarse primero?
```

#### Salidas

```text
reports/security_report.md
reports/critical_nodes.csv
reports/attack_paths.json
reports/components.json
reports/security_summary.md
```

#### Valor profesional

```text
análisis de caminos de ataque
detección de privilegios excesivos
auditoría de dependencias
análisis de propagación lateral
priorización de activos críticos
investigación de fraude o accesos anómalos
```
