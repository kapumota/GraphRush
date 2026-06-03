#[cxx::bridge(namespace = "graphrush")]
pub mod ffi {
    unsafe extern "C++" {
        include!("graphrush/ffi_bridge.hpp");

        type CsrGraph;

        fn load_graph(path: &str) -> Result<UniquePtr<CsrGraph>>;
        fn node_count(graph: &CsrGraph) -> u64;
        fn edge_count(graph: &CsrGraph) -> u64;
        fn max_degree(graph: &CsrGraph) -> u64;
        fn average_degree(graph: &CsrGraph) -> f64;
    }
}
