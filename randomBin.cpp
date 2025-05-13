#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <string>

// Genera una secuencia de números aleatorios de 64 bits
std::vector<int64_t> generarNumerosAleatorios(size_t N) {
    std::vector<int64_t> numeros(N);
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(INT64_MIN, INT64_MAX);

    for (size_t i = 0; i < N; ++i) {
        numeros[i] = dist(gen);
    }
    return numeros;
}

// Escribe una secuencia en un archivo binario
void guardarEnBinario(const std::string& nombreArchivo, const std::vector<int64_t>& secuencia) {
    std::ofstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo) {
        throw std::ios_base::failure("Error al abrir el archivo para escritura");
    }
    archivo.write(reinterpret_cast<const char*>(secuencia.data()), secuencia.size() * sizeof(int64_t));
    archivo.close();
}

int main() {
    try {
        // Tamaños de las secuencias en múltiplos de M
        size_t M = (50 * 1024 * 1024) / sizeof(int64_t);
        std::vector<size_t> tamanos = {4 * M, 8 * M}; // Para pruebas rápidas
        for (size_t tamano : tamanos) {
            for (int i = 0; i < 5; ++i) { // Generar 5 secuencias por tamaño
                std::vector<int64_t> secuencia = generarNumerosAleatorios(tamano);

                // Crear un nombre único para cada archivo
                std::string nombreArchivo = "secuencia_" + std::to_string(tamano / M) + "M_" + std::to_string(i) + ".bin";
                guardarEnBinario(nombreArchivo, secuencia);

                std::cout << "Archivo generado: " << nombreArchivo << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}