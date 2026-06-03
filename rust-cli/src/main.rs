use clap::{Parser, Subcommand};
use std::error::Error;
use std::path::{Path, PathBuf};

mod ffi;

#[derive(Parser)]
#[command(name = "graphrush")]
#[command(version = "0.2.1")]
#[command(about = "Motor híbrido C++/Rust para análisis de grafos.")]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    Import {
        #[arg(long)]
        input: String,

        #[arg(long)]
        output: String,

        #[arg(long, default_value = "edgelist")]
        format: String,

        #[arg(long, default_value_t = true)]
        directed: bool,

        #[arg(long, default_value_t = true)]
        deduplicate: bool,

        #[arg(long, default_value_t = false)]
        weighted: bool,
    },

    Stats {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value_t = false)]
        json: bool,
    },

    Validate {
        #[arg(long)]
        graph: String,
    },

    Bfs {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: u64,

        #[arg(long, default_value_t = 1)]
        threads: u32,

        #[arg(long, default_value_t = false)]
        json: bool,

        #[arg(long)]
        output_csv: Option<String>,
    },

    Components {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value_t = 1)]
        threads: u32,

        #[arg(long, default_value_t = false)]
        json: bool,
    },

    Pagerank {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value_t = 20)]
        iterations: u32,

        #[arg(long, default_value_t = 0.85)]
        damping: f64,

        #[arg(long, default_value_t = 1)]
        threads: u32,

        #[arg(long, default_value_t = 10)]
        top_k: u64,

        #[arg(long, default_value_t = false)]
        json: bool,
    },

    Dijkstra {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: u64,

        #[arg(long, default_value_t = false)]
        json: bool,

        #[arg(long)]
        output_csv: Option<String>,
    },

    ParallelBfs {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: u64,

        #[arg(long, default_value_t = 4)]
        threads: u32,
    },

    ParallelComponents {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value_t = 4)]
        threads: u32,
    },

    ParallelPagerank {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value_t = 20)]
        iterations: u32,

        #[arg(long, default_value_t = 0.85)]
        damping: f64,

        #[arg(long, default_value_t = 4)]
        threads: u32,

        #[arg(long, default_value_t = 10)]
        top_k: u64,
    },

    Benchmark {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value = "bfs,pagerank,components")]
        algos: String,

        #[arg(long, default_value = "1,2,4,8,16")]
        threads: String,

        #[arg(long, default_value = "reports")]
        output: String,

        #[arg(long, default_value_t = 0)]
        source: u64,

        #[arg(long, default_value_t = 20)]
        iterations: u32,

        #[arg(long, default_value_t = 20)]
        top_k: u64,

        #[arg(long, default_value_t = false)]
        use_perf: bool,
    },

    Sssp {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: u64,

        #[arg(long, default_value = "dijkstra")]
        algo: String,

        #[arg(long, default_value_t = 1.0)]
        delta: f64,

        #[arg(long, default_value_t = 1)]
        threads: u32,

        #[arg(long, default_value_t = false)]
        compare: bool,

        #[arg(long)]
        output_csv: Option<String>,
    },
        Security {
        #[command(subcommand)]
        command: SecurityCommands,
    },
}
#[derive(Subcommand)]
enum SecurityCommands {
    Import {
        #[arg(long)]
        input: String,

        #[arg(long)]
        output: String,
    },

    Stats {
        #[arg(long)]
        graph: String,
    },

    Rank {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value = "pagerank")]
        method: String,

        #[arg(long, default_value_t = 50)]
        top: u32,
    },

    Path {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: String,

        #[arg(long)]
        target: String,

        #[arg(long, default_value = "bfs")]
        algo: String,
    },

    Components {
        #[arg(long)]
        graph: String,

        #[arg(long, default_value_t = 10)]
        min_size: u32,
    },

    ExportCsr {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        edge_list: String,

        #[arg(long)]
        mapping: String,

        #[arg(long)]
        output_csr: String,
    },

    CoreRank {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        output_dir: String,

        #[arg(long, default_value_t = 20)]
        top: u32,

        #[arg(long, default_value_t = 20)]
        iterations: u32,

        #[arg(long, default_value_t = 4)]
        threads: u32,
    },

    CorePath {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: String,

        #[arg(long)]
        target: String,

        #[arg(long)]
        output_dir: String,

        #[arg(long, default_value_t = 4)]
        threads: u32,
    },

    CoreSssp {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        source: String,

        #[arg(long)]
        output_dir: String,

        #[arg(long, default_value = "delta")]
        algo: String,

        #[arg(long, default_value_t = 2.0)]
        delta: f64,

        #[arg(long, default_value_t = 4)]
        threads: u32,
    },

    Report {
        #[arg(long)]
        graph: String,

        #[arg(long)]
        output: String,
    },
}

fn main() -> Result<(), Box<dyn Error>> {
    let cli = Cli::parse();

    match cli.command {
        Commands::Import {
            input,
            output,
            format,
            directed,
            deduplicate,
            weighted,
        } => run_import(&input, &output, &format, directed, deduplicate, weighted)?,

        Commands::Stats { graph, json } => run_stats(&graph, json)?,

        Commands::Validate { graph } => run_validate(&graph)?,

        Commands::Bfs {
            graph,
            source,
            threads,
            json,
            output_csv,
        } => run_bfs(&graph, source, threads, json, output_csv.as_deref())?,

        Commands::Components {
            graph,
            threads,
            json,
        } => run_components(&graph, threads, json)?,

        Commands::Pagerank {
            graph,
            iterations,
            damping,
            threads,
            top_k,
            json,
        } => run_pagerank(&graph, iterations, damping, threads, top_k, json)?,

        Commands::Dijkstra {
            graph,
            source,
            json,
            output_csv,
        } => run_dijkstra(&graph, source, json, output_csv.as_deref())?,

        Commands::ParallelBfs {
            graph,
            source,
            threads,
        } => run_parallel_bfs(&graph, source, threads)?,

        Commands::ParallelComponents { graph, threads } => {
            run_parallel_components(&graph, threads)?
        }

        Commands::ParallelPagerank {
            graph,
            iterations,
            damping,
            threads,
            top_k,
        } => run_parallel_pagerank(&graph, iterations, damping, threads, top_k)?,

        Commands::Benchmark {
            graph,
            algos,
            threads,
            output,
            source,
            iterations,
            top_k,
            use_perf,
        } => run_benchmark(&graph, &algos, &threads, &output, source, iterations, top_k, use_perf)?,

        Commands::Sssp {
            graph,
            source,
            algo,
            delta,
            threads,
            compare,
            output_csv,
        } => run_sssp(&graph, source, &algo, delta, threads, compare, output_csv.as_deref())?,

        Commands::Security { command } => run_security(command)?,
    }

    Ok(())
}

fn ensure_file_exists(path: &str) -> Result<(), Box<dyn Error>> {
    if !Path::new(path).exists() {
        return Err(format!("No existe el archivo indicado: {path}").into());
    }

    Ok(())
}

fn load_graph_ref(path: &str) -> Result<cxx::UniquePtr<ffi::ffi::CsrGraph>, Box<dyn Error>> {
    ensure_file_exists(path)?;
    let graph = ffi::ffi::load_graph(path)?;
    Ok(graph)
}
fn resolve_security_script() -> Result<PathBuf, Box<dyn Error>> {
    let current_dir_candidate = PathBuf::from("scripts").join("security_pack.py");
    if current_dir_candidate.exists() {
        return Ok(current_dir_candidate);
    }

    let manifest_candidate = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("..")
        .join("scripts")
        .join("security_pack.py");

    if manifest_candidate.exists() {
        return Ok(manifest_candidate);
    }

    Err("No se encontró scripts/security_pack.py. Ejecute desde la raíz del repositorio o conserve la estructura del proyecto.".into())
}

fn run_security(command: SecurityCommands) -> Result<(), Box<dyn Error>> {
    let script_path = resolve_security_script()?;
    let mut process = std::process::Command::new("python3");
    process.arg(script_path);

    match command {
        SecurityCommands::Import { input, output } => {
            process.arg("import").arg("--input").arg(input).arg("--output").arg(output);
        }

        SecurityCommands::Stats { graph } => {
            process.arg("stats").arg("--graph").arg(graph);
        }

        SecurityCommands::Rank { graph, method, top } => {
            process
                .arg("rank")
                .arg("--graph")
                .arg(graph)
                .arg("--method")
                .arg(method)
                .arg("--top")
                .arg(top.to_string());
        }

        SecurityCommands::Path {
            graph,
            source,
            target,
            algo,
        } => {
            process
                .arg("path")
                .arg("--graph")
                .arg(graph)
                .arg("--source")
                .arg(source)
                .arg("--target")
                .arg(target)
                .arg("--algo")
                .arg(algo);
        }

        SecurityCommands::Components { graph, min_size } => {
            process
                .arg("components")
                .arg("--graph")
                .arg(graph)
                .arg("--min-size")
                .arg(min_size.to_string());
        }

        SecurityCommands::ExportCsr {
            graph,
            edge_list,
            mapping,
            output_csr,
        } => {
            let current_binary = std::env::current_exe()?;
            process
                .arg("export-csr")
                .arg("--graph")
                .arg(graph)
                .arg("--edge-list")
                .arg(edge_list)
                .arg("--mapping")
                .arg(mapping)
                .arg("--output-csr")
                .arg(output_csr)
                .arg("--binary")
                .arg(current_binary);
        }

        SecurityCommands::CoreRank {
            graph,
            output_dir,
            top,
            iterations,
            threads,
        } => {
            let current_binary = std::env::current_exe()?;
            process
                .arg("core-rank")
                .arg("--graph")
                .arg(graph)
                .arg("--output-dir")
                .arg(output_dir)
                .arg("--top")
                .arg(top.to_string())
                .arg("--iterations")
                .arg(iterations.to_string())
                .arg("--threads")
                .arg(threads.to_string())
                .arg("--binary")
                .arg(current_binary);
        }

        SecurityCommands::CorePath {
            graph,
            source,
            target,
            output_dir,
            threads,
        } => {
            let current_binary = std::env::current_exe()?;
            process
                .arg("core-path")
                .arg("--graph")
                .arg(graph)
                .arg("--source")
                .arg(source)
                .arg("--target")
                .arg(target)
                .arg("--output-dir")
                .arg(output_dir)

.arg("--threads")
                .arg(threads.to_string())
                .arg("--binary")
                .arg(current_binary);
        }

        SecurityCommands::CoreSssp {
            graph,
            source,
            output_dir,
            algo,
            delta,
            threads,
        } => {
            let current_binary = std::env::current_exe()?;
            process
                .arg("core-sssp")
                .arg("--graph")
                .arg(graph)
                .arg("--source")
                .arg(source)
                .arg("--output-dir")
                .arg(output_dir)
                .arg("--algo")
                .arg(algo)
                .arg("--delta")
                .arg(delta.to_string())
                .arg("--threads")
                .arg(threads.to_string())
                .arg("--binary")
                .arg(current_binary);
        }

        SecurityCommands::Report { graph, output } => {
            process
                .arg("report")
                .arg("--graph")
                .arg(graph)
                .arg("--output")
                .arg(output);
        }
    }

    let status = process.status()?;
    if !status.success() {
        return Err("GraphRush Security Pack no finalizó correctamente.".into());
    }

    Ok(())
}

fn run_import(
    input: &str,
    output: &str,
    format: &str,
    directed: bool,
    deduplicate: bool,
    weighted: bool,
) -> Result<(), Box<dyn Error>> {
    ensure_file_exists(input)?;

    let stats = ffi::ffi::import_graph(input, output, format, directed, deduplicate, weighted)?;

    println!("[GraphRush] Importación completada correctamente.");
    println!("[GraphRush] Archivo de salida: {output}");
    print_human_stats(&stats);

    Ok(())
}

fn run_stats(path: &str, json: bool) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    let stats = ffi::ffi::graph_stats(graph_ref);

    if json {
        print_json_stats(&stats);
    } else {
        println!("[GraphRush] Grafo cargado correctamente.");
        print_human_stats(&stats);
    }

    Ok(())
}

fn run_validate(path: &str) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    if ffi::ffi::validate_graph(graph_ref) {
        println!("[GraphRush] Validación CSR: correcta.");
    } else {
        println!("[GraphRush] Validación CSR: fallida.");
    }

    Ok(())
}

fn run_bfs(
    path: &str,
    source: u64,
    threads: u32,
    json: bool,
    output_csv: Option<&str>,
) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    if threads > 1 {
        print!("{}", ffi::ffi::run_parallel_bfs_report(graph_ref, source, threads));
    } else if json {
        print!("{}", ffi::ffi::run_bfs_json_report(graph_ref, source));
    } else {
        print!("{}", ffi::ffi::run_bfs_report(graph_ref, source));
    }

    if let Some(csv_path) = output_csv {
        ffi::ffi::write_bfs_distances_csv(graph_ref, source, csv_path)?;
        println!("[GraphRush] Distancias BFS guardadas en: {csv_path}");
    }

    Ok(())
}

fn run_components(path: &str, threads: u32, json: bool) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    if threads > 1 {
        print!("{}", ffi::ffi::run_parallel_components_report(graph_ref, threads));
    } else if json {
        print!("{}", ffi::ffi::run_components_json_report(graph_ref));
    } else {
        print!("{}", ffi::ffi::run_components_report(graph_ref));
    }

    Ok(())
}

fn run_pagerank(
    path: &str,
    iterations: u32,
    damping: f64,
    threads: u32,
    top_k: u64,
    json: bool,
) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    if threads > 1 {
        print!(
            "{}",
            ffi::ffi::run_parallel_pagerank_report(
                graph_ref,
                iterations,
                damping,
                threads,
                top_k,
            )
        );
    } else if json {
        print!(
            "{}",
            ffi::ffi::run_pagerank_json_report(graph_ref, iterations, damping, top_k)
        );
    } else {
        print!(
            "{}",
            ffi::ffi::run_pagerank_top_report(graph_ref, iterations, damping, top_k)
        );
    }

    Ok(())
}

fn run_dijkstra(
    path: &str,
    source: u64,
    json: bool,
    output_csv: Option<&str>,
) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    if json {
        print!("{}", ffi::ffi::run_dijkstra_json_report(graph_ref, source));
    } else {
        print!("{}", ffi::ffi::run_dijkstra_report(graph_ref, source));
    }

    if let Some(csv_path) = output_csv {
        ffi::ffi::write_dijkstra_distances_csv(graph_ref, source, csv_path)?;
        println!("[GraphRush] Distancias Dijkstra guardadas en: {csv_path}");
    }

    Ok(())
}



fn resolve_benchmark_script() -> Result<PathBuf, Box<dyn Error>> {
    let current_dir_candidate = PathBuf::from("scripts").join("benchmark_engine.py");
    if current_dir_candidate.exists() {
        return Ok(current_dir_candidate);
    }

    let manifest_candidate = PathBuf::from(env!("CARGO_MANIFEST_DIR"))
        .join("..")
        .join("scripts")
        .join("benchmark_engine.py");

    if manifest_candidate.exists() {
        return Ok(manifest_candidate);
    }

    Err("No se encontró scripts/benchmark_engine.py. Ejecute desde la raíz del repositorio o conserve la estructura del proyecto.".into())
}


fn run_sssp(
    path: &str,
    source: u64,
    algorithm: &str,
    delta: f64,
    threads: u32,
    compare: bool,
    output_csv: Option<&str>,
) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    print!(
        "{}",
        ffi::ffi::run_sssp_report(
            graph_ref,
            algorithm,
            source,
            delta,
            threads,
            compare,
        )
    );

    if let Some(csv_path) = output_csv {
        ffi::ffi::write_sssp_distances_csv(
            graph_ref,
            algorithm,
            source,
            delta,
            threads,
            csv_path,
        )?;
        println!("[GraphRush] Distancias SSSP guardadas en: {csv_path}");
    }

    Ok(())
}


fn run_benchmark(
    graph: &str,
    algos: &str,
    threads: &str,
    output: &str,
    source: u64,
    iterations: u32,
    top_k: u64,
    use_perf: bool,
) -> Result<(), Box<dyn Error>> {
    ensure_file_exists(graph)?;

    let script_path = resolve_benchmark_script()?;
    let current_binary = std::env::current_exe()?;

    let mut command = std::process::Command::new("python3");
    command
        .arg(script_path)
        .arg("--binary")
        .arg(current_binary)
        .arg("--graph")
        .arg(graph)
        .arg("--algos")
        .arg(algos)
        .arg("--threads")
        .arg(threads)
        .arg("--output")
        .arg(output)
        .arg("--source")
        .arg(source.to_string())
        .arg("--iterations")
        .arg(iterations.to_string())
        .arg("--top-k")
        .arg(top_k.to_string());

    if use_perf {
        command.arg("--use-perf");
    }

    let status = command.status()?;
    if !status.success() {
        return Err("El Benchmark Engine no finalizó correctamente.".into());
    }

    Ok(())
}


fn run_parallel_bfs(path: &str, source: u64, threads: u32) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    print!("{}", ffi::ffi::run_parallel_bfs_report(graph_ref, source, threads));
    Ok(())
}

fn run_parallel_components(path: &str, threads: u32) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    print!("{}", ffi::ffi::run_parallel_components_report(graph_ref, threads));
    Ok(())
}

fn run_parallel_pagerank(
    path: &str,
    iterations: u32,
    damping: f64,
    threads: u32,
    top_k: u64,
) -> Result<(), Box<dyn Error>> {
    let graph = load_graph_ref(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    print!(
        "{}",
        ffi::ffi::run_parallel_pagerank_report(
            graph_ref,
            iterations,
            damping,
            threads,
            top_k
        )
    );
    Ok(())
}


fn print_human_stats(stats: &ffi::ffi::GraphStats) {
    println!("[GraphRush] Nodos: {}", stats.nodes);
    println!("[GraphRush] Aristas: {}", stats.edges);
    println!("[GraphRush] Grado máximo: {}", stats.max_degree);
    println!("[GraphRush] Grado promedio: {:.4}", stats.average_degree);
    println!("[GraphRush] Memoria aproximada: {} bytes", stats.memory_bytes);
    println!("[GraphRush] Tiempo de carga: {:.4} ms", stats.load_time_ms);
    println!("[GraphRush] CSR válido: {}", if stats.valid { "sí" } else { "no" });
    println!("[GraphRush] Grafo ponderado: {}", if stats.weighted { "sí" } else { "no" });
}

fn print_json_stats(stats: &ffi::ffi::GraphStats) {
    println!("{{");
    println!("  \"nodes\": {},", stats.nodes);
    println!("  \"edges\": {},", stats.edges);
    println!("  \"max_degree\": {},", stats.max_degree);
    println!("  \"average_degree\": {:.6},", stats.average_degree);
    println!("  \"memory_bytes\": {},", stats.memory_bytes);
    println!("  \"load_time_ms\": {:.6},", stats.load_time_ms);
    println!("  \"valid\": {},", stats.valid);
    println!("  \"weighted\": {}", stats.weighted);
    println!("}}");
}
