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
