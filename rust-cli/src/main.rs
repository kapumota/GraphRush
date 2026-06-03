use clap::{Parser, Subcommand};
use std::error::Error;
use std::path::Path;

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
        } => run_import(&input, &output, &format, directed, deduplicate)?,

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

fn run_import(
    input: &str,
    output: &str,
    format: &str,
    directed: bool,
    deduplicate: bool,
) -> Result<(), Box<dyn Error>> {
    ensure_file_exists(input)?;

    let stats = ffi::ffi::import_graph(input, output, format, directed, deduplicate)?;

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
}

fn print_json_stats(stats: &ffi::ffi::GraphStats) {
    println!("{{");
    println!("  \"nodes\": {},", stats.nodes);
    println!("  \"edges\": {},", stats.edges);
    println!("  \"max_degree\": {},", stats.max_degree);
    println!("  \"average_degree\": {:.6},", stats.average_degree);
    println!("  \"memory_bytes\": {},", stats.memory_bytes);
    println!("  \"load_time_ms\": {:.6},", stats.load_time_ms);
    println!("  \"valid\": {}", stats.valid);
    println!("}}");
}
