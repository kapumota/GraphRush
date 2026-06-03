#[cxx::bridge(namespace = "graphrush")]
pub mod ffi {
    struct GraphStats {
        nodes: u64,
        edges: u64,
        max_degree: u64,
        average_degree: f64,
        memory_bytes: u64,
        load_time_ms: f64,
        valid: bool,
    }

    unsafe extern "C++" {
        include!("graphrush/ffi_bridge.hpp");

        type CsrGraph;

        fn load_graph(path: &str) -> Result<UniquePtr<CsrGraph>>;

        fn import_graph(
            input_path: &str,
            output_path: &str,
            format: &str,
            directed: bool,
            deduplicate: bool,
        ) -> Result<GraphStats>;

        fn graph_stats(graph: &CsrGraph) -> GraphStats;

        fn validate_graph(graph: &CsrGraph) -> bool;
    }
}
