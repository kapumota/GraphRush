### Diseño de GraphRush Security Pack

#### Arquitectura

El Security Pack se implementa como runner Python invocado desde la CLI Rust.

```text
graphrush security ... -> Rust CLI -> scripts/security_pack.py -> reportes
```

#### Formato interno

El comando `security import` convierte CSV a un archivo JSON con extensión `.gr`:

```json
{
  "schema_version": "graphrush.security.v1",
  "nodes": ["user_15", "group_admin"],
  "edges": [
    {
      "source": "user_15",
      "target": "group_admin",
      "relation": "member_of",
      "weight": 1.0
    }
  ]
}
```

#### Algoritmos usados

```text
PageRank para nodos críticos
BFS para caminos de ataque no ponderados
Dijkstra para caminos ponderados
componentes conexas débiles para segmentos
```

#### Razón de diseño

El core C++ de GraphRush trabaja con IDs numéricos. Para ciberseguridad, los nombres de nodos son entidades semánticas importantes. Por eso la Fase 6 conserva nombres simbólicos en el Security Pack y genera reportes legibles para analistas.

#### Extensión futura

Una fase posterior puede mapear el grafo de seguridad a `.grcsr` numérico para ejecutar directamente los algoritmos C++ de alto rendimiento.
