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
