#include "mergesort.h"
#include <iostream>
#include <string>


/**
 * Punto de entrada principal del programa.
 *
 * Este programa realiza el ordenamiento externo de un archivo binario de entrada
 * utilizando el algoritmo de MergeSort externo. El archivo ordenado se guarda
 * en un archivo de salida especificado por el usuario.
 *
 * @param argc Número de argumentos pasados al programa.
 * @param argv Array de cadenas que contiene los argumentos:
 *             - argv[1]: Nombre del archivo de entrada.
 *             - argv[2]: Nombre del archivo de salida.
 * @return Código de salida del programa:
 *         - 0 si la ejecución fue exitosa.
 *         - 1 si ocurrió un error.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <archivo_entrada> <archivo_salida>" << std::endl;
        return 1;
    }

    std::string archivoEntrada = argv[1];
    std::string archivoSalida = argv[2];

    try {
        external_mergesort(archivoEntrada, archivoSalida, 50); // Se puede cambiar a.
        std::cout << "Archivo ordenado generado: " << archivoSalida << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}