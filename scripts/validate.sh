#!/usr/bin/env bash
set -euo pipefail

# Validacion reproducible para GraphRush.
# Comprueba estructura, core C++20, CLI Rust, scripts Python y demos minimas.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

TMP_DIR="${TMPDIR:-/tmp}/graphrush-validation"
GRAPH_FILE="$TMP_DIR/example.grcsr"
SECURITY_GRAPH="$TMP_DIR/security.gr"

cleanup() {
  rm -rf "$TMP_DIR"
}
trap cleanup EXIT

mkdir -p "$TMP_DIR"

echo "[GraphRush] 1. Verificando estructura minima"
test -f README.md
test -f LICENSE
test -f Makefile
test -f CHANGELOG.md
test -f cpp-core/CMakeLists.txt
test -f rust-cli/Cargo.toml
test -f rust-cli/Cargo.lock
test -d cpp-core/include/graphrush
test -d cpp-core/src
test -d cpp-core/tests
test -d data/small
test -d data/control
test -d data/security
test -d scripts

echo "[GraphRush] 2. Verificando que no existan artefactos generados versionados"
if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  if git ls-files | grep -E '(^|/)(__pycache__|target|build|\.pytest_cache)(/|$)|\.(pyc|pyo|o|a|so|dylib|dll|exe|out)$|\.(grcsr|gr)$' > "$TMP_DIR/generated_files.txt"; then
    echo "Error: hay artefactos generados versionados:"
    cat "$TMP_DIR/generated_files.txt"
    exit 1
  fi
else
  echo "Advertencia: no se detecto un repositorio git. Se omite control de artefactos versionados."
fi

echo "[GraphRush] 3. Validando scripts Python"
python3 -m py_compile scripts/security_pack.py
python3 -m py_compile scripts/benchmark_engine.py
python3 -m py_compile scripts/phase3_parallel_benchmark.py
python3 -m py_compile scripts/generate_security_graph.py

echo "[GraphRush] 4. Compilando y probando core C++20"
cmake -S cpp-core -B build/cpp -DCMAKE_BUILD_TYPE=Release
cmake --build build/cpp -j
ctest --test-dir build/cpp --output-on-failure

echo "[GraphRush] 5. Compilando y probando CLI Rust"
(
  cd rust-cli
  cargo build --release
  cargo test
  cargo clippy --all-targets -- -D warnings
)

echo "[GraphRush] 6. Ejecutando demo minima de importacion y analisis"
./rust-cli/target/release/graphrush-cli import \
  --input data/small/example.edges \
  --format snap \
  --output "$GRAPH_FILE" \
  --directed \
  --deduplicate

test -s "$GRAPH_FILE"

./rust-cli/target/release/graphrush-cli stats \
  --graph "$GRAPH_FILE" \
  --json > "$TMP_DIR/stats.json"

test -s "$TMP_DIR/stats.json"

./rust-cli/target/release/graphrush-cli validate \
  --graph "$GRAPH_FILE" > "$TMP_DIR/validate.txt"

test -s "$TMP_DIR/validate.txt"

./rust-cli/target/release/graphrush-cli bfs \
  --graph "$GRAPH_FILE" \
  --source 0 > "$TMP_DIR/bfs.txt"

test -s "$TMP_DIR/bfs.txt"

./rust-cli/target/release/graphrush-cli pagerank \
  --graph "$GRAPH_FILE" \
  --iterations 10 \
  --top-k 3 > "$TMP_DIR/pagerank.txt"

test -s "$TMP_DIR/pagerank.txt"

echo "[GraphRush] 7. Ejecutando demo minima del Security Pack"
./rust-cli/target/release/graphrush-cli security import \
  --input data/security/security_edges.csv \
  --output "$SECURITY_GRAPH"

test -s "$SECURITY_GRAPH"

./rust-cli/target/release/graphrush-cli security stats \
  --graph "$SECURITY_GRAPH" > "$TMP_DIR/security_stats.txt"

test -s "$TMP_DIR/security_stats.txt"

./rust-cli/target/release/graphrush-cli security rank \
  --graph "$SECURITY_GRAPH" \
  --method pagerank \
  --top 5 > "$TMP_DIR/security_rank.txt"

test -s "$TMP_DIR/security_rank.txt"

echo "[GraphRush] Validacion completada correctamente."
