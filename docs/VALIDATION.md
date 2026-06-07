### Validación reproducible

#### Objetivo

Este documento describe la validación mínima reproducible de GraphRush como software híbrido C++20 y Rust para procesamiento de grafos, benchmarking y análisis de ciberseguridad.

La validación no depende únicamente de badges visuales en el README. El badge principal de validación apunta a un workflow real de GitHub Actions. Ese workflow ejecuta `scripts/validate.sh`, que comprueba compilación, pruebas, CLI, scripts Python y demos mínimas.

#### Comando principal

```bash
make validate
```

También puede ejecutarse directamente:

```bash
bash scripts/validate.sh
```

#### Qué se valida

- Estructura mínima del repositorio.
- Existencia de `README.md`, `LICENSE`, `CHANGELOG.md`, `Makefile`, `cpp-core`, `rust-cli`, `data` y `scripts`.
- Ausencia de artefactos generados versionados, como `build`, `target`, `__pycache__`, `.pyc`, `.grcsr` o `.gr`.
- Sintaxis de scripts Python mediante `python3 -m py_compile`.
- Configuración, compilación y pruebas del core C++20 mediante CMake y CTest.
- Compilación, pruebas y análisis estático de la CLI Rust mediante Cargo y Clippy.
- Importación de un grafo pequeño a formato `.grcsr`.
- Ejecución de `stats`, `validate`, `bfs` y `pagerank` sobre el grafo importado.
- Importación y análisis mínimo del Security Pack.

#### Relación con los badges

El README muestra badges para resumir el estado técnico del proyecto. Los badges más importantes son `CI` y `validation`, porque dependen de workflows reales de GitHub Actions.

Si `scripts/validate.sh` falla, el workflow de validación falla y el badge deja de mostrar un estado exitoso.

#### Alcance

Esta validación demuestra que GraphRush funciona como software compilable, testeable y ejecutable en un entorno limpio de CI. No reemplaza una evaluación de rendimiento con datasets masivos ni una comparación completa con frameworks externos como GAP Benchmark Suite, Graph500 o Ligra.
