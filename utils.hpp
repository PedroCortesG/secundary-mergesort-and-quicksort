#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <cstdint>

/**
 * Genera un archivo binario con una cantidad específica de números aleatorios.
 *
 * @param filename Nombre del archivo binario a generar.
 * @param cantidad Cantidad de números aleatorios a escribir en el archivo.
 */
void generarArchivoBinario(const std::string& filename, size_t cantidad);

/**
 * Lee un bloque de números enteros de un archivo binario.
 *
 * @param in Flujo de entrada desde el cual se leerán los datos.
 * @param cantidad Cantidad de números a leer del archivo.
 * @return std::vector<uint32_t> Vector que contiene los números leídos del archivo.
 */
std::vector<uint32_t> leerBloque(std::ifstream& in, size_t cantidad);

/**
 * Escribe un bloque de números enteros en un archivo binario.
 *
 * @param out Flujo de salida al cual se escribirán los datos.
 * @param datos Vector que contiene los números a escribir en el archivo.
 */
void escribirBloque(std::ofstream& out, const std::vector<uint32_t>& datos);

/**
 * Imprime el contenido de un archivo binario en la consola.
 *
 * @param filename Nombre del archivo binario a leer.
 * @param max_elementos Máximo número de elementos a imprimir (por defecto 20).
 */
void imprimirArchivoBinario(const std::string& filename, size_t max_elementos = 20);

#endif
