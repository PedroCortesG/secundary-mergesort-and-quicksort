#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

// Constantes
constexpr size_t B = 4096;            // Tamaño de bloque en bytes
constexpr size_t INT_SIZE = 4;        // Tamaño de un entero en bytes
constexpr size_t INTS_PER_BLOCK = B / INT_SIZE;

// Funciones de lectura y escritura por bloque
int read_block(int* buffer, FILE* file, size_t& io_count);         // Devuelve cuántos int leyó
void write_block(const int* buffer, int count, FILE* file, size_t& io_count);

// Utilidades
size_t count_ints_in_file(const std::string& filename);

#endif
