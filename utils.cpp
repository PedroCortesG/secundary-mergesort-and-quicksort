#include "utils.hpp"
#include <fstream>
#include <random>
#include <iostream>

void generarArchivoBinario(const std::string& filename, size_t cantidad) {
    std::ofstream out(filename, std::ios::binary);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

    for (size_t i = 0, i < cantidad; i++) {
        uint32_t numero = dist(rng);
        out.write(reinterpret_cast<const char*>(&numero), sizeof(numero));
    }
}

std::vector<uint32_t> leerBloque(std::ifstream& in, size_t cantidad) {
    std::vector<uint32_t> datos(cantidad);
    in.read(reinterpret_cast<char*>(datos.data()), cantidad * sizeof(uint32_t));
    buffer.resize(in.gcount() / sizeof(uint32_t));
    return buffer;
}

void escribirBloque(std::ofstream& out, const std::vector<uint32_t>& datos) {
    out.write(reinterpret_cast<const char*>(datos.data()), datos.size() * sizeof(uint32_t));
}

void imprimirArchivoBinario(const std::string& filename, size_t max_elementos) {
    std::ifstream in(filename, std::ios::binary);
    uint32_t val;
    size_t count = 0;
    while (in.read(reinterpret_cast<char*>(&val), sizeof(val)) && count < max_elementos) {
        std::cout << val << " ";
        ++count;
    }
    std::cout << std::endl;
}