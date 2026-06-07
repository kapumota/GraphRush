.PHONY: build-cpp test-cpp build-rust demo-import demo-stats demo-json demo-validate clean

build-cpp:
	cmake -S cpp-core -B build/cpp -DCMAKE_BUILD_TYPE=Release
	cmake --build build/cpp -j

test-cpp: build-cpp
	ctest --test-dir build/cpp --output-on-failure

build-rust:
	cd rust-cli && cargo build

demo-import:
	cd rust-cli && cargo run -- import --input ../data/small/example.edges --format snap --output ../data/small/example.grcsr --directed --deduplicate

demo-stats:
	cd rust-cli && cargo run -- stats --graph ../data/small/example.grcsr

demo-json:
	cd rust-cli && cargo run -- stats --graph ../data/small/example.grcsr --json

demo-validate:
	cd rust-cli && cargo run -- validate --graph ../data/small/example.grcsr

clean:
	rm -rf build
	cd rust-cli && cargo clean
	rm -f data/small/example.grcsr


.PHONY: demo-bfs demo-components demo-pagerank demo-dijkstra test-phase2

demo-bfs:
	cd rust-cli && cargo run -- bfs --graph ../data/small/example.grcsr --source 0

demo-components:
	cd rust-cli && cargo run -- components --graph ../data/small/example.grcsr

demo-pagerank:
	cd rust-cli && cargo run -- pagerank --graph ../data/small/example.grcsr --iterations 20

demo-dijkstra:
	cd rust-cli && cargo run -- dijkstra --graph ../data/small/example.grcsr --source 0

test-phase2: test-cpp


.PHONY: demo-bfs-json demo-bfs-csv demo-pagerank-top demo-pagerank-json demo-dijkstra-json demo-dijkstra-csv

demo-bfs-json:
	cd rust-cli && cargo run -- bfs --graph ../data/small/example.grcsr --source 0 --json

demo-bfs-csv:
	mkdir -p reports
	cd rust-cli && cargo run -- bfs --graph ../data/small/example.grcsr --source 0 --output-csv ../reports/bfs_distances.csv

demo-pagerank-top:
	cd rust-cli && cargo run -- pagerank --graph ../data/small/example.grcsr --iterations 20 --top-k 5

demo-pagerank-json:
	cd rust-cli && cargo run -- pagerank --graph ../data/small/example.grcsr --iterations 20 --top-k 5 --json

demo-dijkstra-json:
	cd rust-cli && cargo run -- dijkstra --graph ../data/small/example.grcsr --source 0 --json

demo-dijkstra-csv:
	mkdir -p reports
	cd rust-cli && cargo run -- dijkstra --graph ../data/small/example.grcsr --source 0 --output-csv ../reports/dijkstra_distances.csv


.PHONY: demo-parallel-bfs demo-parallel-components demo-parallel-pagerank

demo-parallel-bfs:
	cd rust-cli && cargo run -- parallel-bfs --graph ../data/small/example.grcsr --source 0 --threads 4

demo-parallel-components:
	cd rust-cli && cargo run -- parallel-components --graph ../data/small/example.grcsr --threads 4

demo-parallel-pagerank:
	cd rust-cli && cargo run -- parallel-pagerank --graph ../data/small/example.grcsr --iterations 20 --threads 4 --top-k 5


.PHONY: demo-phase3-benchmark

demo-phase3-benchmark:
	python scripts/phase3_parallel_benchmark.py \
		--binary rust-cli/target/release/graphrush \
		--graph data/small/example.grcsr \
		--algos bfs,pagerank,components \
		--threads 1,2,4,8,16 \
		--source 0 \
		--iterations 20 \
		--output reports/phase3


.PHONY: demo-phase3-memory

demo-phase3-memory:
	python scripts/phase3_parallel_benchmark.py \
		--binary rust-cli/target/release/graphrush \
		--graph data/small/example.grcsr \
		--algos bfs,pagerank,components \
		--threads 1,2,4,8,16 \
		--source 0 \
		--iterations 20 \
		--output reports/phase3 \
		--measure-memory


.PHONY: demo-phase4-benchmark

demo-phase4-benchmark:
	python scripts/benchmark_engine.py \
		--binary rust-cli/target/release/graphrush \
		--graph data/small/example.grcsr \
		--algos bfs,pagerank,components \
		--threads 1,2,4,8,16 \
		--output reports


.PHONY: demo-phase4-small

demo-phase4-small:
	cd rust-cli && cargo build --release
	./rust-cli/target/release/graphrush-cli import \
		--input data/small/example.edges \
		--format snap \
		--output data/small/example.grcsr \
		--directed \
		--deduplicate
	./rust-cli/target/release/graphrush-cli benchmark \
		--graph data/small/example.grcsr \
		--algos bfs,pagerank,components \
		--threads 1,2,4,8,16 \
		--output reports/


.PHONY: demo-phase5-sssp

demo-phase5-sssp:
	cd rust-cli && cargo build --release
	./rust-cli/target/release/graphrush-cli import \
		--input data/control/sssp_unit.edges \
		--format snap \
		--output data/control/sssp_unit.grcsr \
		--directed \
		--deduplicate
	./rust-cli/target/release/graphrush-cli sssp \
		--graph data/control/sssp_unit.grcsr \
		--source 0 \
		--algo delta \
		--delta 4 \
		--threads 4 \
		--compare \
		--output-csv reports/sssp_distances.csv


.PHONY: demo-phase5-weighted

demo-phase5-weighted:
	cd rust-cli && cargo build --release
	./rust-cli/target/release/graphrush-cli import \
		--input data/control/weighted_sssp.edges \
		--format snap \
		--output data/control/weighted_sssp.grcsr \
		--directed \
		--deduplicate \
		--weighted
	./rust-cli/target/release/graphrush-cli sssp \
		--graph data/control/weighted_sssp.grcsr \
		--source 0 \
		--algo delta \
		--delta 2 \
		--threads 4 \
		--compare \
		--output-csv reports/weighted_delta.csv

.PHONY: validate

validate:
	bash scripts/validate.sh
