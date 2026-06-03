.PHONY: build-cpp test-cpp build-rust demo clean

build-cpp:
	cmake -S cpp-core -B build/cpp -DCMAKE_BUILD_TYPE=Release
	cmake --build build/cpp -j

test-cpp: build-cpp
	ctest --test-dir build/cpp --output-on-failure

build-rust:
	cd rust-cli && cargo build

demo:
	cd rust-cli && cargo run -- stats --graph ../data/small/example.edges

clean:
	rm -rf build
	cd rust-cli && cargo clean
