use clap::{Parser, Subcommand};
use std::error::Error;
use std::path::Path;

mod ffi;

#[derive(Parser)]
#[command(name = "graphrush")]
#[command(version = "0.1.1")]
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
    }

    Ok(())
}

fn ensure_file_exists(path: &str) -> Result<(), Box<dyn Error>> {
    if !Path::new(path).exists() {
        return Err(format!("No existe el archivo indicado: {path}").into());
    }

    Ok(())
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
    ensure_file_exists(path)?;

    let graph = ffi::ffi::load_graph(path)?;
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
    ensure_file_exists(path)?;

    let graph = ffi::ffi::load_graph(path)?;
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
