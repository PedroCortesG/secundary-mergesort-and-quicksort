#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <cstdio>
#include <unordered_map>
#include <unordered_set>
#include "utils.h"

using namespace std;

const int a = 100; // número de particiones (modificable)


size_t get_file_size(const std::string &filename) {
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    return in.tellg(); // Retorna posición final = tamaño
}


// Elige a-1 pivotes aleatorios desde un bloque del archivo
vector<int> choose_pivots(const string& filename) {
    ifstream fin(filename, ios::binary);
    vector<int> buffer(M);
    uniform_int_distribution<int> distrib(0, M - 1);
    random_device rd;
    mt19937 gen(rd());

    // Lee un bloque aleatorio
    int total_blocks = get_file_size(filename) / (M * sizeof(int));
    int block_index = gen() % max(total_blocks, 1);
    fin.seekg(block_index * M * sizeof(int));
    fin.read(reinterpret_cast<char*>(buffer.data()), M * sizeof(int));

    // Selecciona a-1 elementos aleatorios
    vector<int> pivots;
    unordered_set<int> chosen;
    while ((int)pivots.size() < a - 1) {
        int idx = distrib(gen);
        if (chosen.insert(idx).second) {
            pivots.push_back(buffer[idx]);
        }
    }
    sort(pivots.begin(), pivots.end());
    return pivots;
}

// Determina la partición a la que pertenece un valor dado los pivotes
int get_partition(int value, const vector<int>& pivots) {
    int i = 0;
    while (i < (int)pivots.size() && value > pivots[i]) ++i;
    return i;
}

// Divide el archivo original en a archivos temporales según los pivotes
vector<string> partition_file(const string& filename, const vector<int>& pivots) {
    ifstream fin(filename, ios::binary);
    vector<ofstream> tmp_files(a);
    vector<string> tmp_names(a);

    for (int i = 0; i < a; ++i) {
        tmp_names[i] = filename + ".part" + to_string(i);
        tmp_files[i].open(tmp_names[i], ios::binary);
    }

    vector<int> buffer(M);
    while (fin.read(reinterpret_cast<char*>(buffer.data()), M * sizeof(int)) || fin.gcount()) {
        int count = fin.gcount() / sizeof(int);
        for (int i = 0; i < count; ++i) {
            int part = get_partition(buffer[i], pivots);
            tmp_files[part].write(reinterpret_cast<char*>(&buffer[i]), sizeof(int));
        }
    }

    for (auto& f : tmp_files) f.close();
    return tmp_names;
}

// Ordena un archivo pequeño que cabe en memoria
void sort_small_file(const string& filename) {
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

// Concatenar archivos en uno solo
void concatenate_files(const vector<string>& files, const string& output) {
    ofstream fout(output, ios::binary | ios::trunc);
    vector<int> buffer(M);
    for (const string& file : files) {
        ifstream fin(file, ios::binary);
        while (fin.read(reinterpret_cast<char*>(buffer.data()), M * sizeof(int)) || fin.gcount()) {
            fout.write(reinterpret_cast<char*>(buffer.data()), fin.gcount());
        }
        fin.close();
        remove(file.c_str());
    }
    fout.close();
}

void external_quicksort(const string& filename) {
    size_t file_size = get_file_size(filename);
    size_t num_ints = file_size / sizeof(int);

    if (num_ints <= (size_t)M) {
        sort_small_file(filename);
        return;
    }

    vector<int> pivots = choose_pivots(filename);
    vector<string> parts = partition_file(filename, pivots);

    for (const string& part : parts) {
        external_quicksort(part);
    }

    concatenate_files(parts, filename);
}

// main de ejemplo para probar
int main(int argc, char** argv) {
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <archivo>" << endl;
        return 1;
    }
    external_quicksort(argv[1]);
    return 0;
}
