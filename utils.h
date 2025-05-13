#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint> // Para int64_t

// Parámetros globales
extern size_t B; // Tamaño de bloque en enteros
extern size_t M; // Tamaño de memoria en enteros

// Contadores de accesos
extern size_t disk_reads;
extern size_t disk_writes;

// Inicializa contadores
void reset_counters();

// Lee hasta B enteros de 64 bits desde un archivo binario
std::vector<int64_t> read_block(std::ifstream &file);

// Escribe un bloque de enteros de 64 bits en un archivo binario
void write_block(std::ofstream &file, const std::vector<int64_t> &block);

// Escribe un entero de 64 bits en binario
void write_int(std::ofstream &file, int64_t value);

// Lee un entero de 64 bits en binario
bool read_int(std::ifstream &file, int64_t &value);

// Devuelve el número de lecturas de disco
size_t get_read_count();

// Devuelve el número de escrituras de disco
size_t get_write_count();

#endif // UTILS_H