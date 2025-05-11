#include "utils.hpp"
#include <cstdio>

int read_block(int* buffer, FILE* file, size_t& io_count) {
    int read = fread(buffer, INT_SIZE, INTS_PER_BLOCK, file);
    if (read > 0) ++io_count;
    return read;
}

void write_block(const int* buffer, int count, FILE* file, size_t& io_count) {
    if (count > 0) {
        fwrite(buffer, INT_SIZE, count, file);
        ++io_count;
    }
}

size_t count_ints_in_file(const std::string& filename) {
    FILE* file = fopen(filename.c_str(), "rb");
    if (!file) return 0;
    fseek(file, 0, SEEK_END);
    size_t bytes = ftell(file);
    fclose(file);
    return bytes / INT_SIZE;
}
