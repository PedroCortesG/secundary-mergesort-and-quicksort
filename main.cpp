#include "mergesort.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_entrada> <archivo_salida>" << std::endl;
        return 1;
    }

    std::string archivoEntrada = argv[1];
    std::string archivoSalida = argv[2];

    try {
        external_mergesort(archivoEntrada, archivoSalida);
        std::cout << "Archivo ordenado generado: " << archivoSalida << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}