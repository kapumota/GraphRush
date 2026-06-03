#!/usr/bin/env bash
set -euo pipefail

echo "[GraphRush] Limpiando temporales..."

find . -type d -name "__pycache__" -prune -exec rm -rf {} +
find . -type f -name "*.pyc" -delete

echo "[GraphRush] Verificando C++..."

cmake -S cpp-core -B build/cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build/cpp -j
ctest --test-dir build/cpp --output-on-failure

echo "[GraphRush] Verificando Rust..."

if command -v cargo >/dev/null 2>&1; then
  cd rust-cli
  cargo build
  cargo run -- stats --graph ../data/small/example.edges
  cd ..
else
  echo "[GraphRush] Cargo no está instalado. Se omite verificación Rust."
fi

echo "[GraphRush] Verificación local completada correctamente."
