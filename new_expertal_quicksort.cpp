#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <cstdio>
#include <unordered_set>
#include <stdexcept>
#include "utils.h"

using namespace std;

const int a = 100; // número de particiones

size_t get_file_size(const string &filename) {
    ifstream in(filename, ios::binary | ios::ate);
    return static_cast<size_t>(in.tellg());
}

vector<int> choose_pivots(const string &filename) {
    ifstream fin(filename, ios::binary);
    if (!fin) throw runtime_error("No se pudo abrir el archivo para elegir pivotes.");

    vector<int> buffer(M);
    uniform_int_distribution<int> distrib(0, M - 1);
    random_device rd;
    mt19937 gen(rd());

    int total_blocks = get_file_size(filename) / (M * sizeof(int));
    int block_index = gen() % max(total_blocks, 1);
    fin.seekg(block_index * M * sizeof(int));
    fin.read(reinterpret_cast<char*>(buffer.data()), M * sizeof(int));
    size_t read_elems = fin.gcount() / sizeof(int);

    if (read_elems == 0) throw runtime_error("No se pudieron leer datos para pivotes.");

    vector<int> pivots;
    unordered_set<int> chosen;
    while ((int)pivots.size() < a - 1 && (int)chosen.size() < (int)read_elems) {
        int idx = distrib(gen) % read_elems;
        if (chosen.insert(idx).second) {
            pivots.push_back(buffer[idx]);
        }
    }

    sort(pivots.begin(), pivots.end());
    return pivots;
}

int get_partition(int value, const vector<int> &pivots) {
    int i = 0;
    while (i < (int)pivots.size() && value > pivots[i]) ++i;
    return i;
}

vector<string> partition_file(const string &filename, const vector<int> &pivots) {
    ifstream fin(filename, ios::binary);
    if (!fin) throw runtime_error("No se pudo abrir el archivo para particionar.");

    vector<ofstream> tmp_files(a);
    vector<string> tmp_names(a);

    for (int i = 0; i < a; ++i) {
        tmp_names[i] = filename + ".part" + to_string(i);
        tmp_files[i].open(tmp_names[i], ios::binary);
    }

    vector<int> buffer(M);
    while (true) {
        fin.read(reinterpret_cast<char*>(buffer.data()), M * sizeof(int));
        streamsize bytes = fin.gcount();
        if (bytes == 0) break;

        int count = bytes / sizeof(int);
        for (int i = 0; i < count; ++i) {
            int part = get_partition(buffer[i], pivots);
            tmp_files[part].write(reinterpret_cast<char*>(&buffer[i]), sizeof(int));
        }
    }

    for (auto &f : tmp_files) f.close();
    fin.close();
    remove(filename.c_str()); // Elimina el archivo original para evitar reprocesarlo
    return tmp_names;
}

void sort_small_file(const string &filename) {
    ifstream fin(filename, ios::binary);
    vector<int> buffer;
    int x;
    while (fin.read(reinterpret_cast<char*>(&x), sizeof(int))) {
        buffer.push_back(x);
    }
    fin.close();

    sort(buffer.begin(), buffer.end());

    ofstream fout(filename, ios::binary | ios::trunc);
    for (int val : buffer) {
        fout.write(reinterpret_cast<char*>(&val), sizeof(int));
    }
    fout.close();
}

void concatenate_files(const vector<string> &files, const string &output) {
    ofstream fout(output, ios::binary | ios::trunc);
    vector<int> buffer(M);
    for (const string &file : files) {
        ifstream fin(file, ios::binary);
        while (true) {
            fin.read(reinterpret_cast<char*>(buffer.data()), M * sizeof(int));
            streamsize bytes = fin.gcount();
            if (bytes == 0) break;
            fout.write(reinterpret_cast<char*>(buffer.data()), bytes);
        }
        fin.close();
        remove(file.c_str()); // Elimina cada partición después de usarla
    }
    fout.close();
}

void external_quicksort(const string &filename) {
    size_t file_size = get_file_size(filename);
    size_t num_ints = file_size / sizeof(int);

    if (num_ints <= (size_t)M) {
        sort_small_file(filename);
        return;
    }

    vector<int> pivots = choose_pivots(filename);
    vector<string> parts = partition_file(filename, pivots);

    for (const string &part : parts) {
        if (get_file_size(part) > 0) {
            external_quicksort(part);
        } else {
            remove(part.c_str());
        }
    }

    concatenate_files(parts, filename);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <archivo>" << endl;
        return 1;
    }

    external_quicksort(argv[1]);
    return 0;
}
