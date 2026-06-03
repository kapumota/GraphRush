### Diseño de GraphRush

#### Resumen

GraphRush usa una arquitectura híbrida C++20/Rust. C++20 contiene el core de grafos y Rust contiene la CLI, validación, configuración y reportes.

#### Decisiones principales

```text
C++20: núcleo de grafos y algoritmos
Rust: CLI, reportes, validación y configuración
Python: scripts auxiliares
Formato base: EdgeList / SNAP / CSV
Representación interna: CSR
Formato binario propio: .grcsr
```

#### Fase 1.1

La Fase 1.1 fortalece la ingesta de datos antes de iniciar los algoritmos:

- `GraphStats` incluye memoria aproximada.
- `GraphStats` incluye tiempo de carga.
- `stats` permite salida humana y salida JSON.
- Rust valida rutas antes de llamar al core C++.
- El formato `.grcsr` evita parsear texto en cada ejecución.

#### Razón técnica

Los datasets reales en texto pueden ser costosos de parsear. Por eso GraphRush sigue una estrategia profesional:

```text
texto real → parser robusto → normalización → CSR → binario .grcsr → algoritmos
```

Esta idea está inspirada en motores y suites de grafos como Ligra, GBBS y GAP Benchmark Suite, donde se separa la conversión del grafo del procesamiento algorítmico.
