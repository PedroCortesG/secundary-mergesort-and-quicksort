#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <string>

// Genera una secuencia de números enteros de 64 bits
std::vector<int64_t> generarSecuencia(size_t N) {
    std::vector<int64_t> secuencia(N);
    std::iota(secuencia.begin(), secuencia.end(), 1); // Rellena con 1, 2, ..., N
    return secuencia;
}

// Desordena una secuencia
void desordenarSecuencia(std::vector<int64_t>& secuencia) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(secuencia.begin(), secuencia.end(), gen);
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
        // Todos juntos toma mucho espacio
        //std::vector<size_t> tamanos = {4 * M, 8 * M, 12 * M, 16 * M, 20 * M, 24 * M, 28 * M, 32 * M, 36 * M, 40 * M, 44 * M, 48 * M, 52 * M, 56 * M, 60 * M};
        std::vector<size_t> tamanos = {4 * M, 8 * M}; // Para pruebas rápidas
        for (size_t tamano : tamanos) {
            for (int i = 0; i < 5; ++i) { // Generar 5 secuencias por tamaño
                std::vector<int64_t> secuencia = generarSecuencia(tamano);
                desordenarSecuencia(secuencia);

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