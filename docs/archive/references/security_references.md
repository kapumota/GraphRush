### Referencias de ciberseguridad basada en grafos

#### BloodHound y Attack Path Management

BloodHound popularizó el uso de grafos para representar relaciones en Active Directory y analizar caminos de ataque, privilegios excesivos y rutas hacia activos críticos.

#### Attack graphs

Los attack graphs modelan posibles rutas de compromiso, relaciones de privilegio, vulnerabilidades y estados de un atacante. GraphRush toma esta idea para construir `security path`, `security rank` y `security components`.

#### Graph-based intrusion detection

Los enfoques de detección basada en grafos modelan logs, hosts, usuarios, procesos y comunicaciones como estructuras relacionales. GraphRush Security Pack se limita al análisis estructural reproducible, no a detección automática con machine learning.

#### Decisión de alcance

GraphRush Security Pack no ejecuta explotación, no reemplaza un SIEM, no recolecta credenciales y no automatiza acciones ofensivas. Solo analiza relaciones declaradas en CSV.
