#include "utils.h"
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <chrono>

namespace fs = std::filesystem;

static size_t run_counter = 0;

/**
 * Crea runs iniciales ordenados que caben en memoria principal.
 *
 * @param input_file Nombre del archivo binario de entrada.
 * @return Vector con los nombres de los archivos de los runs creados.
 */

std::vector<std::string> create_initial_runs(const std::string &input_file) {
    std::vector<std::string> runs;
    std::ifstream input(input_file, std::ios::binary);

    while (!input.eof()) {
        std::vector<int64_t> buffer;
        int64_t value;
        for (size_t i = 0; i < M && read_int(input, value); ++i) {
            buffer.push_back(value);
        }

        if (!buffer.empty()) {
            std::sort(buffer.begin(), buffer.end());
            std::string run_file = "run_" + std::to_string(run_counter++) + ".bin";
            std::ofstream output(run_file, std::ios::binary);
            write_block(output, buffer);
            output.close();
            runs.push_back(run_file);
        }
    }

    input.close();
    return runs;
}

/**
 * Mezcla K runs ordenados en un único archivo.
 *
 * @param runs Vector con los nombres de los archivos de los runs a mezclar.
 * @return Nombre del archivo resultante de la mezcla.
 */
std::string merge_runs(const std::vector<std::string> &runs) {
    using HeapNode = std::pair<int64_t, size_t>;
    auto cmp = [](HeapNode a, HeapNode b) { return a.first > b.first; };
    std::priority_queue<HeapNode, std::vector<HeapNode>, decltype(cmp)> heap(cmp);

    size_t K = runs.size();
    std::vector<std::ifstream> run_streams(K);
    std::vector<int64_t> current(K);
    std::vector<bool> has_value(K, false);

    // Inicializa el heap con el primer elemento de cada run
    for (size_t i = 0; i < K; ++i) {
        run_streams[i].open(runs[i], std::ios::binary);
        if (read_int(run_streams[i], current[i])) {
            has_value[i] = true;
            heap.emplace(current[i], i);
        }
    }

    std::string output_file = "merged_" + std::to_string(run_counter++) + ".bin";
    std::ofstream output(output_file, std::ios::binary);

    // Mezcla los runs usando un heap
    while (!heap.empty()) {
        auto [min_val, idx] = heap.top();
        heap.pop();
        write_int(output, min_val);
        if (read_int(run_streams[idx], current[idx])) {
            heap.emplace(current[idx], idx);
        }
    }

    // Cierra los streams y elimina los archivos temporales
    for (auto &rs : run_streams) {
        rs.close();
    }

    output.close();

    for (const auto &f : runs) {
        fs::remove(f);
    }

    return output_file;
}

/**
 * Implementa el algoritmo de MergeSort externo.
 *
 * @param input_file Nombre del archivo binario de entrada.
 * @param output_file Nombre del archivo binario de salida ordenado.
 * @param a Aridad.
 */
void external_mergesort(const std::string &input_file, const std::string &output_file, size_t a) {
    using namespace std::chrono;

    reset_counters();
    auto start = high_resolution_clock::now();

    // Crea los runs iniciales
    std::vector<std::string> runs = create_initial_runs(input_file);

    // Mezcla los runs hasta que quede un único archivo
    while (runs.size() > 1) {
        std::vector<std::string> new_runs;
        for (size_t i = 0; i < runs.size(); i += a) {
            std::vector<std::string> group(runs.begin() + i,
                                           runs.begin() + std::min(i + a, runs.size()));
            new_runs.push_back(merge_runs(group));
        }
        runs = std::move(new_runs);
    }

    // Renombra el archivo final
    if (!runs.empty()) {
        fs::rename(runs[0], output_file);
    }

    auto end = high_resolution_clock::now();
    auto duration_ms = duration_cast<milliseconds>(end - start).count();

    // Reporta estadísticas
    std::cout << "Tiempo total: " << duration_ms << " ms" << std::endl;
    std::cout << "Lecturas de disco: " << get_read_count() << std::endl;
    std::cout << "Escrituras de disco: " << get_write_count() << std::endl;
    std::cout << "I/O's totales: " << get_read_count() + get_write_count() << std::endl;
}