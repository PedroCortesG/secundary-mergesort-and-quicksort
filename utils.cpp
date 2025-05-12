#include "utils.h"
#include <iostream>
#include <cstring>

size_t B = 1024;
size_t M = (50 * 1024 * 1024) / sizeof(int64_t);

size_t disk_reads = 0;
size_t disk_writes = 0;

void reset_counters() {
    disk_reads = 0;
    disk_writes = 0;
}

std::vector<int> read_block(std::ifstream &file) {
    std::vector<int> block(B);
    file.read(reinterpret_cast<char *>(block.data()), B * sizeof(int));
    size_t read_count = file.gcount() / sizeof(int);
    block.resize(read_count);
    if (read_count > 0) {
        disk_reads++;
    }
    return block;
}

void write_block(std::ofstream &file, const std::vector<int> &block) {
    file.write(reinterpret_cast<const char *>(block.data()), block.size() * sizeof(int));
    if (!block.empty()) {
        disk_writes++;
    }
}

void write_int(std::ofstream &file, int value) {
    file.write(reinterpret_cast<const char *>(&value), sizeof(int));
    disk_writes++;
}

bool read_int(std::ifstream &file, int &value) {
    file.read(reinterpret_cast<char *>(&value), sizeof(int));
    if (file.gcount() == sizeof(int)) {
        disk_reads++;
        return true;
    }
    return false;
}
