### Fase 6 - GraphRush Security Pack

#### Objetivo

Demostrar una aplicación profesional de GraphRush en ciberseguridad. El módulo Security Pack analiza relaciones entre usuarios, máquinas, servicios, permisos, recursos críticos, IPs y procesos.

#### Modelo de grafo

```text
usuario → grupo
grupo → permiso
usuario → máquina
máquina → servicio
servicio → base de datos
IP → dominio
proceso → archivo
proceso → conexión
cuenta → recurso crítico
```

#### Formato de entrada

```csv
source,target,relation,weight
user_15,group_admin,member_of,1
group_admin,server_db,has_access,1
machine_22,user_15,logged_in_as,1
ip_10.0.0.5,machine_22,connects_to,1
```

#### Comandos esperados

```bash
graphrush security import --input security_edges.csv --output security.gr
graphrush security stats --graph security.gr
graphrush security rank --graph security.gr --method pagerank --top 50
graphrush security path --graph security.gr --source attacker_machine --target database_server
graphrush security report --graph security.gr --output reports/security_report.md
```

#### Preguntas profesionales

```text
¿Qué usuarios tienen mayor influencia?
¿Cuál es el camino más corto hacia un servidor crítico?
¿Qué grupos concentran demasiados permisos?
¿Qué máquinas conectan componentes sensibles?
¿Qué nodos deberían auditarse primero?
```

#### Salidas

```text
reports/security_report.md
reports/critical_nodes.csv
reports/attack_paths.json
reports/components.json
```
