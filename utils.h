#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <fstream>

// Parámetros globales
extern size_t B; // Tamaño de bloque en enteros
extern size_t M; // Tamaño de memoria en enteros

// Contadores de accesos
extern size_t disk_reads;
extern size_t disk_writes;

// Inicializa contadores
void reset_counters();

// Lee hasta B enteros desde un archivo binario
std::vector<int> read_block(std::ifstream &file);

// Escribe un bloque de enteros en un archivo binario
void write_block(std::ofstream &file, const std::vector<int> &block);

// Escribe un entero en binario
void write_int(std::ofstream &file, int value);

// Lee un entero en binario
bool read_int(std::ifstream &file, int &value);

#endif // UTILS_H
