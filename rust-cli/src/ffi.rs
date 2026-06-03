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

        fn run_bfs_report(graph: &CsrGraph, source: u64) -> String;
        fn run_components_report(graph: &CsrGraph) -> String;
        fn run_dijkstra_report(graph: &CsrGraph, source: u64) -> String;

        fn run_bfs_json_report(graph: &CsrGraph, source: u64) -> String;
        fn run_components_json_report(graph: &CsrGraph) -> String;
        fn run_pagerank_top_report(graph: &CsrGraph, iterations: u32, damping: f64, top_k: u64) -> String;
        fn run_pagerank_json_report(graph: &CsrGraph, iterations: u32, damping: f64, top_k: u64) -> String;
        fn run_dijkstra_json_report(graph: &CsrGraph, source: u64) -> String;

        fn write_bfs_distances_csv(graph: &CsrGraph, source: u64, output_path: &str) -> Result<()>;
        fn write_dijkstra_distances_csv(graph: &CsrGraph, source: u64, output_path: &str) -> Result<()>;

        fn run_parallel_bfs_report(graph: &CsrGraph, source: u64, threads: u32) -> String;
        fn run_parallel_components_report(graph: &CsrGraph, threads: u32) -> String;
        fn run_parallel_pagerank_report(graph: &CsrGraph, iterations: u32, damping: f64, threads: u32, top_k: u64) -> String;
    }
}
