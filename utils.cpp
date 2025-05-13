#include "utils.h"
#include <cstdint>
#include <fstream>
#include <iostream>

size_t B = 4096; // Sacado con el comando especificado en el foro
size_t M = (50 * 1024 * 1024) / sizeof(int64_t); // (50MB)

// Para poder ver I/O'S.
size_t disk_reads = 0;
size_t disk_writes = 0;

// Inicializa y resetea contadores
void reset_counters() {
    disk_reads = 0;
    disk_writes = 0;
}

/**
 * Lee un bloque de datos desde un archivo de entrada. Cuenta las lecturas de disco.
 * @param file Archivo de entrada desde el cual se leerá.
 * @return Vector de enteros de 64 bits que contiene los datos leídos.
 *         El tamaño del vector puede ser menor que el tamaño del bloque si no hay suficientes datos.
 */
std::vector<int64_t> read_block(std::ifstream &file) {
    std::vector<int64_t> block(B);
    file.read(reinterpret_cast<char *>(block.data()), B * sizeof(int64_t));
    size_t read_count = file.gcount() / sizeof(int64_t);
    block.resize(read_count);
    if (read_count > 0) {
        disk_reads++;
    }
    return block;
}

/**
 * Escribe un bloque de datos en un archivo de salida. Cuenta las escrituras de disco.
 * @param file Archivo de salida donde se escribirá.
 * @param block Vector de enteros de 64 bits que contiene los datos a escribir.
 */
void write_block(std::ofstream &file, const std::vector<int64_t> &block) {
    file.write(reinterpret_cast<const char *>(block.data()), block.size() * sizeof(int64_t));
    if (!block.empty()) {
        disk_writes++;
    }
}

/**
 * Escribe un entero de 64 bits en un archivo de salida.
 * @param file Archivo de salida donde se escribirá.
 * @param value Entero de 64 bits que se escribirá en el archivo.
 */
void write_int(std::ofstream &file, int64_t value) {
    file.write(reinterpret_cast<const char *>(&value), sizeof(int64_t));
    disk_writes++;
}

/**
 * Lee un entero de 64 bits desde un archivo de entrada.
 * @param file Archivo de entrada desde el cual se leerá.
 * @param value Referencia donde se almacenará el entero leído.
 * @return `true` si se leyó un entero correctamente, `false` si no hay más datos.
 */
bool read_int(std::ifstream &file, int64_t &value) {
    file.read(reinterpret_cast<char *>(&value), sizeof(int64_t));
    if (file.gcount() == sizeof(int64_t)) {
        disk_reads++;
        return true;
    }
    return false;
}

/**
 * Obtiene el número total de lecturas realizadas desde el disco.
 * @return Número de lecturas realizadas.
 */
size_t get_read_count() {
    return disk_reads;
}

/**
 * Obtiene el número total de escrituras realizadas en el disco.
 * @return Número de escrituras realizadas.
 */
size_t get_write_count() {
    return disk_writes;
}