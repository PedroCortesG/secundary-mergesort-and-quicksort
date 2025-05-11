#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>
#include <queue>
#include "utils.hpp"

using namespace std;

void create_sorted_partitions(const string& input_file, int a, vector<string>& partition_names, size_t& io_count) {
    FILE* in = fopen(input_file.c_str(), "rb");
    if (!in) {
        cerr << "No se pudo abrir archivo de entrada.\n";
        exit(1);
    }

    int* buffer = new int[INTS_PER_BLOCK];
    size_t total_read = 0;
    int part_id = 0;

    while (true) {
        vector<int> chunk;
        int read_count = 0;
        while (read_count < a * INTS_PER_BLOCK) {
            int r = fread(buffer, INT_SIZE, INTS_PER_BLOCK, in);
            if (r <= 0) break;
            io_count++;
            chunk.insert(chunk.end(), buffer, buffer + r);
            read_count += r;
        }
        if (chunk.empty()) break;

        sort(chunk.begin(), chunk.end());

        string part_name = "partition_" + to_string(part_id++) + ".bin";
        partition_names.push_back(part_name);

        FILE* out = fopen(part_name.c_str(), "wb");
        for (size_t i = 0; i < chunk.size(); i += INTS_PER_BLOCK) {
            int count = min((size_t)INTS_PER_BLOCK, chunk.size() - i);
            write_block(&chunk[i], count, out, io_count);
        }
        fclose(out);
    }

    delete[] buffer;
    fclose(in);
}

struct HeapItem {
    int value;
    int file_index;

    bool operator>(const HeapItem& other) const {
        return value > other.value;
    }
};

void merge_partitions(const vector<string>& partition_names, const string& output_file, size_t& io_count) {
    int k = partition_names.size();
    vector<FILE*> files(k);
    vector<int*> buffers(k);
    vector<int> buffer_sizes(k, 0);
    vector<int> buffer_pos(k, 0);

    for (int i = 0; i < k; ++i) {
        files[i] = fopen(partition_names[i].c_str(), "rb");
        buffers[i] = new int[INTS_PER_BLOCK];
        buffer_sizes[i] = fread(buffers[i], INT_SIZE, INTS_PER_BLOCK, files[i]);
        io_count++;
    }

    FILE* out = fopen(output_file.c_str(), "wb");
    int* out_buffer = new int[INTS_PER_BLOCK];
    int out_pos = 0;

    auto cmp = greater<HeapItem>();
    priority_queue<HeapItem, vector<HeapItem>, decltype(cmp)> heap(cmp);

    for (int i = 0; i < k; ++i) {
        if (buffer_sizes[i] > 0) {
            heap.push({buffers[i][0], i});
            buffer_pos[i] = 1;
        }
    }

    while (!heap.empty()) {
        auto [val, idx] = heap.top();
        heap.pop();
        out_buffer[out_pos++] = val;

        if (out_pos == INTS_PER_BLOCK) {
            write_block(out_buffer, INTS_PER_BLOCK, out, io_count);
            out_pos = 0;
        }

        if (buffer_pos[idx] == buffer_sizes[idx]) {
            buffer_sizes[idx] = fread(buffers[idx], INT_SIZE, INTS_PER_BLOCK, files[idx]);
            io_count++;
            buffer_pos[idx] = 0;
        }

        if (buffer_sizes[idx] > 0) {
            heap.push({buffers[idx][buffer_pos[idx]++], idx});
        }
    }

    if (out_pos > 0) write_block(out_buffer, out_pos, out, io_count);

    for (int i = 0; i < k; ++i) {
        fclose(files[i]);
        delete[] buffers[i];
    }

    delete[] out_buffer;
    fclose(out);
}

void mergesortExterno(const string& input_file, const string& output_file, size_t& io_count) {
    size_t num_ints = count_ints_in_file(input_file);
    size_t num_blocks = (num_ints + INTS_PER_BLOCK - 1) / INTS_PER_BLOCK;
    int a = max(2, (int)sqrt(num_blocks));  // como indica el enunciado

    vector<string> partition_names;
    create_sorted_partitions(input_file, a, partition_names, io_count);
    merge_partitions(partition_names, output_file, io_count);

    // Elimina archivos temporales
    for (const auto& name : partition_names) {
        remove(name.c_str());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: ./mergesort <archivo_entrada> <archivo_salida>\n";
        return 1;
    }

    size_t io_count = 0;
    mergesortExterno(argv[1], argv[2], io_count);

    cout << "Total de accesos a disco: " << io_count << "\n";
    return 0;
}
