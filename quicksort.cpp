#include "utils.h"
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <chrono>

namespace fs = std::filesystem;

static size_t quick_counter = 0;

// Selecciona a-1 pivotes aleatorios desde un bloque del archivo
std::vector<int64_t> select_pivots(std::ifstream &file, size_t a) {
    std::vector<int64_t> buffer;
    int64_t val;

    // Leer un bloque al azar (simple: el primero)
    for (size_t i = 0; i < B && read_int(file, val); ++i) {
        buffer.push_back(val);
    }

    std::shuffle(buffer.begin(), buffer.end(), std::mt19937(std::random_device()()));
    std::sort(buffer.begin(), buffer.begin() + std::min(a - 1, buffer.size()));

    buffer.resize(std::min(a - 1, buffer.size()));
    return buffer;
}

// Clasifica cada valor en una de las a particiones según pivotes
size_t classify(int64_t value, const std::vector<int64_t> &pivots) {
    size_t i = 0;
    while (i < pivots.size() && value > pivots[i]) ++i;
    return i;
}

// Quicksort externo
void external_quicksort(const std::string &input_file, const std::string &output_file) {
    std::ifstream input(input_file, std::ios::binary | std::ios::ate);
    size_t file_size = input.tellg();
    size_t num_elements = file_size / sizeof(int64_t);
    input.seekg(0);

    if (num_elements <= M) {
        // Leer a memoria, ordenar y escribir
        std::vector<int64_t> buffer;
        int64_t val;
        while (read_int(input, val)) {
            buffer.push_back(val);
        }
        input.close();

        std::sort(buffer.begin(), buffer.end());

        std::ofstream output(output_file, std::ios::binary);
        write_block(output, buffer);
        output.close();
        return;
    }

    // Elegir a-1 pivotes
    size_t a = M / B;
    std::vector<int64_t> pivots = select_pivots(input, a);
    input.clear(); input.seekg(0); // Reiniciar lectura

    // Crear archivos temporales por partición
    std::vector<std::string> partitions(a);
    std::vector<std::ofstream> out_streams(a);

    for (size_t i = 0; i < a; ++i) {
        partitions[i] = "qpart_" + std::to_string(quick_counter++) + "_" + std::to_string(i) + ".bin";
        out_streams[i].open(partitions[i], std::ios::binary);
    }

    // Distribuir los elementos
    int64_t value;
    while (read_int(input, value)) {
        size_t idx = classify(value, pivots);
        write_int(out_streams[idx], value);
    }

    input.close();
    for (auto &out : out_streams) out.close();

    // Ordenar recursivamente y concatenar
    std::ofstream output(output_file, std::ios::binary);
    for (const auto &part_file : partitions) {
        std::string sorted_part = "sorted_" + part_file;
        external_quicksort(part_file, sorted_part);

        std::ifstream sorted_in(sorted_part, std::ios::binary);
        int64_t v;
        while (read_int(sorted_in, v)) {
            write_int(output, v);
        }
        sorted_in.close();

        fs::remove(part_file);
        fs::remove(sorted_part);
    }

    output.close();
    std::cout << "Tiempo total: " << ms << " ms\n";
    std::cout << "Lecturas de disco: " << get_read_count() << "\n";
    std::cout << "Escrituras de disco: " << get_write_count() << "\n";

}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_entrada> <archivo_salida>" << std::endl;
        return 1;
    }

    std::string archivoEntrada = argv[1];
    std::string archivoSalida = argv[2];

    try {
        external_quicksort(archivoEntrada, archivoSalida); // Se puede cambiar a.
        std::cout << "Archivo ordenado generado: " << archivoSalida << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

