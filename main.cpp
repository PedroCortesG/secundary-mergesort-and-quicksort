#include "mergesort.h"
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>


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

    if (true) {
        std::ofstream logFile("tiempos_mergesort.txt");
        if (!logFile) {
            std::cerr << "Error al crear el archivo de log." << std::endl;
            return 1;
        }

        try {
            for (size_t a = 2; a <= 512; ++a) {
                auto start = std::chrono::high_resolution_clock::now();

                std::cout << "Ejecutando external_mergesort con a = " << a << std::endl;
                external_mergesort(archivoEntrada, archivoSalida, a);

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                std::cout << "Archivo ordenado generado con a = " << a << ": " << archivoSalida << std::endl;
                logFile << "a = " << a << ", tiempo = " << duration << " ms" << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }

        logFile.close();
    }

    try {
        external_mergesort(archivoEntrada, archivoSalida, 50); // Se puede cambiar a.
        std::cout << "Archivo ordenado generado: " << archivoSalida << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}