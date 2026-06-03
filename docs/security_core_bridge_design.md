### Diseño del Security Core Bridge

#### Mapeo simbólico a numérico

Cada nodo simbólico recibe un ID entero:

```json
{
  "node_to_id": {
    "attacker_machine": 0,
    "database_server": 4
  },
  "id_to_node": {
    "0": "attacker_machine",
    "4": "database_server"
  }
}
```

#### EdgeList numérico ponderado

El CSV simbólico se convierte a:

```text
0 1 1.0
1 2 1.0
2 3 1.0
```

Este formato permite importar con:

```bash
graphrush import --weighted true
```

#### Ejecución en core C++

Los comandos `core-rank`, `core-path` y `core-sssp` invocan el binario GraphRush:

```text
pagerank -> core C++
bfs      -> core C++
sssp     -> core C++
```

#### Reportes interpretables

Después de ejecutar el core, los IDs numéricos se traducen otra vez a nombres simbólicos usando `security_mapping.json`.
