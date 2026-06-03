#include "graphrush/BinaryGraphIO.hpp"

#include <array>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace graphrush {

namespace {

constexpr std::array<char, 8> MAGIC = {'G', 'R', 'C', 'S', 'R', '0', '1', '\0'};
constexpr std::uint64_t VERSION_UNWEIGHTED = 1;
constexpr std::uint64_t VERSION_WEIGHTED = 2;
constexpr std::uint64_t FLAG_WEIGHTED = 1;

void write_u64(std::ofstream& output, std::uint64_t value) {
    output.write(reinterpret_cast<const char*>(&value), sizeof(value));
}

std::uint64_t read_u64(std::ifstream& input) {
    std::uint64_t value = 0;
    input.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (!input) {
        throw std::runtime_error("No se pudo leer un entero del archivo binario.");
    }
    return value;
}

} // namespace

void BinaryGraphWriter::write(const CsrGraph& graph, const std::string& path) {
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("No se pudo escribir el archivo binario: " + path);
    }

    output.write(MAGIC.data(), static_cast<std::streamsize>(MAGIC.size()));

    const bool weighted = graph.has_weights();
    write_u64(output, weighted ? VERSION_WEIGHTED : VERSION_UNWEIGHTED);
    write_u64(output, graph.node_count());
    write_u64(output, graph.edge_count());

    if (weighted) {
        write_u64(output, FLAG_WEIGHTED);
    }

    const auto& offsets = graph.offsets();
    const auto& neighbors = graph.neighbors();

    output.write(
        reinterpret_cast<const char*>(offsets.data()),
        static_cast<std::streamsize>(offsets.size() * sizeof(std::uint64_t))
    );

    output.write(
        reinterpret_cast<const char*>(neighbors.data()),
        static_cast<std::streamsize>(neighbors.size() * sizeof(std::uint64_t))
    );

    if (weighted) {
        const auto& weights = graph.weights();
        output.write(
            reinterpret_cast<const char*>(weights.data()),
            static_cast<std::streamsize>(weights.size() * sizeof(double))
        );
    }

    if (!output) {
        throw std::runtime_error("No se pudo completar la escritura del archivo binario.");
    }
}

std::unique_ptr<CsrGraph> BinaryGraphReader::read(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("No se pudo abrir el archivo binario: " + path);
    }

    std::array<char, 8> magic{};
    input.read(magic.data(), static_cast<std::streamsize>(magic.size()));
    if (magic != MAGIC) {
        throw std::runtime_error("El archivo no tiene formato GraphRush CSR.");
    }

    const std::uint64_t version = read_u64(input);
    if (version != VERSION_UNWEIGHTED && version != VERSION_WEIGHTED) {
        throw std::runtime_error("Versión de formato GraphRush no soportada.");
    }

    const std::uint64_t nodes = read_u64(input);
    const std::uint64_t edges = read_u64(input);

    bool weighted = false;
    if (version == VERSION_WEIGHTED) {
        const auto flags = read_u64(input);
        weighted = (flags & FLAG_WEIGHTED) != 0;
    }

    std::vector<std::uint64_t> offsets(nodes + 1, 0);
    std::vector<std::uint64_t> neighbors(edges, 0);
    std::vector<double> weights;

    input.read(
        reinterpret_cast<char*>(offsets.data()),
        static_cast<std::streamsize>(offsets.size() * sizeof(std::uint64_t))
    );

    input.read(
        reinterpret_cast<char*>(neighbors.data()),
        static_cast<std::streamsize>(neighbors.size() * sizeof(std::uint64_t))
    );

    if (weighted) {
        weights.assign(edges, 1.0);
        input.read(
            reinterpret_cast<char*>(weights.data()),
            static_cast<std::streamsize>(weights.size() * sizeof(double))
        );
    }

    if (!input) {
        throw std::runtime_error("Archivo binario incompleto o corrupto.");
    }

    return std::make_unique<CsrGraph>(
        std::move(offsets),
        std::move(neighbors),
        std::move(weights)
    );
}

} // namespace graphrush
