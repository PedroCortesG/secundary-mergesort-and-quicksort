#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

int run_counter = 0; // Define run_counter globally

// Divide el archivo de entrada en menores y mayores/iguales al pivote
void dividirArchivo(const std::string &archivoEntrada, int64_t pivote,
                    const std::string &archivoMenores, const std::string &archivoMayores) {
    std::ifstream entrada(archivoEntrada, std::ios::binary);
    std::ofstream menores(archivoMenores, std::ios::binary);
    std::ofstream mayores(archivoMayores, std::ios::binary);

    std::vector<int> buffer = read_block(entrada);
    while (!buffer.empty()) {
        for (int64_t numero : buffer) {
            if (numero < pivote) {
                write_int(menores, numero);
            } else if (numero > pivote) {
                write_int(mayores, numero);
            }
        }
        buffer = read_block(entrada);
    }

    entrada.close();
    menores.close();
    mayores.close();
}

// Cuenta cuántas veces aparece el pivote
size_t contarPivotes(const std::string &archivoEntrada, int64_t pivote) {
    std::ifstream entrada(archivoEntrada, std::ios::binary);
    size_t conteo = 0;

    std::vector<int> buffer = read_block(entrada);
    while (!buffer.empty()) {
        for (int64_t numero : buffer) {
            if (numero == pivote) {
                conteo++;
            }
        }
        buffer = read_block(entrada);
    }

    entrada.close();
    return conteo;
}

// Une los archivos ordenados menores, pivotes y mayores
void unirArchivos(const std::string &archivoSalida,
                  const std::string &archivoMenores, int64_t pivote, size_t repeticiones,
                  const std::string &archivoMayores) {
    std::ofstream salida(archivoSalida, std::ios::binary);
    std::ifstream menores(archivoMenores, std::ios::binary);
    std::ifstream mayores(archivoMayores, std::ios::binary);

    std::vector<int> buffer;

    // Escribir menores
    buffer = read_block(menores);
    while (!buffer.empty()) {
        write_block(salida, buffer);
        buffer = read_block(menores);
    }

    // Escribir pivotes
    for (size_t i = 0; i < repeticiones; ++i) {
        write_int(salida, pivote);
    }

    // Escribir mayores
    buffer = read_block(mayores);
    while (!buffer.empty()) {
        write_block(salida, buffer);
        buffer = read_block(mayores);
    }

    salida.close();
    menores.close();
    mayores.close();
}

// Quicksort externo recursivo
void quicksort_externo(const std::string &archivoEntrada, const std::string &archivoSalida) {
    std::ifstream entrada(archivoEntrada, std::ios::binary | std::ios::ate);
    size_t tamano = entrada.tellg();
    entrada.seekg(0);
    size_t cantidad = tamano / sizeof(int64_t);

    if (cantidad <= 1) {
        std::ofstream salida(archivoSalida, std::ios::binary);
        entrada.seekg(0);
        salida << entrada.rdbuf();
        entrada.close();
        salida.close();
        return;
    }

    int64_t pivote;
    entrada.read(reinterpret_cast<char *>(&pivote), sizeof(int64_t));
    entrada.close();

    std::string menores = "run_" + std::to_string(run_counter++) + "_menores.bin";
    std::string mayores = "run_" + std::to_string(run_counter++) + "_mayores.bin";

    dividirArchivo(archivoEntrada, pivote, menores, mayores);
    size_t cantidadPivotes = contarPivotes(archivoEntrada, pivote);

    // Validar si los archivos menores y mayores están vacíos
    std::ifstream archivoMenores(menores, std::ios::binary | std::ios::ate);
    std::ifstream archivoMayores(mayores, std::ios::binary | std::ios::ate);
    size_t tamanoMenores = archivoMenores.tellg();
    size_t tamanoMayores = archivoMayores.tellg();
    archivoMenores.close();
    archivoMayores.close();

    if (tamanoMenores == 0 && tamanoMayores == 0) {
        std::ofstream salida(archivoSalida, std::ios::binary);
        for (size_t i = 0; i < cantidadPivotes; ++i) {
            write_int(salida, pivote);
        }
        salida.close();
        fs::remove(menores);
        fs::remove(mayores);
        return;
    }

    std::string menoresOrdenado = "run_" + std::to_string(run_counter++) + "_menores_ordenado.bin";
    std::string mayoresOrdenado = "run_" + std::to_string(run_counter++) + "_mayores_ordenado.bin";

    if (tamanoMenores > 0) {
        quicksort_externo(menores, menoresOrdenado);
    }
    if (tamanoMayores > 0) {
        quicksort_externo(mayores, mayoresOrdenado);
    }

    unirArchivos(archivoSalida, menoresOrdenado, pivote, cantidadPivotes, mayoresOrdenado);

    fs::remove(menores);
    fs::remove(mayores);
    fs::remove(menoresOrdenado);
    fs::remove(mayoresOrdenado);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_entrada> <archivo_salida>" << std::endl;
        return 1;
    }

    std::string archivoEntrada = argv[1];
    std::string archivoSalida = argv[2];

    quicksort_externo(archivoEntrada, archivoSalida);

    std::cout << "Archivo ordenado guardado en: " << archivoSalida << std::endl;
    return 0;
}