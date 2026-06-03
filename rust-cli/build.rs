fn main() {
    cxx_build::bridge("src/ffi.rs")
        .file("../cpp-core/src/CsrGraph.cpp")
        .file("../cpp-core/src/ffi_bridge.cpp")
        .include("../cpp-core/include")
        .flag_if_supported("-std=c++20")
        .compile("graphrush_ffi");

    println!("cargo:rerun-if-changed=src/ffi.rs");
    println!("cargo:rerun-if-changed=../cpp-core/include/graphrush/CsrGraph.hpp");
    println!("cargo:rerun-if-changed=../cpp-core/include/graphrush/ffi_bridge.hpp");
    println!("cargo:rerun-if-changed=../cpp-core/src/CsrGraph.cpp");
    println!("cargo:rerun-if-changed=../cpp-core/src/ffi_bridge.cpp");
}
