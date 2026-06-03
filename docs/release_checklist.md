### Checklist de release

#### Validación local

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure

cd rust-cli
cargo build --release
cargo test
cargo clippy --all-targets -- -D warnings
cd ..
```

#### Demo final

```bash
graphrush import --input web-Google.txt --format snap --output web-google.gr
graphrush pagerank --graph web-google.gr --iterations 30 --threads 8 --top 20

graphrush import --input roadNet-CA.txt --format snap --output road-ca.gr
graphrush sssp --graph road-ca.gr --source 0 --algo delta --delta 4 --threads 8

graphrush security import --input security_edges.csv --output security.gr
graphrush security report --graph security.gr --output reports/security_report.md
```

#### Release

```bash
git tag v0.7.0
git push origin v0.7.0
```
