use clap::{Parser, Subcommand};
use std::error::Error;

mod ffi;

#[derive(Parser)]
#[command(name = "graphrush")]
#[command(version = "0.1.0")]
#[command(about = "Motor híbrido C++/Rust para análisis de grafos.")]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    Stats {
        #[arg(long)]
        graph: String,
    },
}

fn main() -> Result<(), Box<dyn Error>> {
    let cli = Cli::parse();

    match cli.command {
        Commands::Stats { graph } => run_stats(&graph)?,
    }

    Ok(())
}

fn run_stats(path: &str) -> Result<(), Box<dyn Error>> {
    let graph = ffi::ffi::load_graph(path)?;
    let graph_ref = graph
        .as_ref()
        .ok_or("No se pudo obtener una referencia válida al grafo.")?;

    println!("[GraphRush] Grafo cargado correctamente.");
    println!("[GraphRush] Nodos: {}", ffi::ffi::node_count(graph_ref));
    println!("[GraphRush] Aristas: {}", ffi::ffi::edge_count(graph_ref));
    println!("[GraphRush] Grado máximo: {}", ffi::ffi::max_degree(graph_ref));
    println!(
        "[GraphRush] Grado promedio: {:.4}",
        ffi::ffi::average_degree(graph_ref)
    );

    Ok(())
}
