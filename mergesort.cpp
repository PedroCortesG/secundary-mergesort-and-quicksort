#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <queue>
#include <cstdio>
#include <algorithm>
#include <limits>

using namespace std;

// Parámetros globales
int M = 1024; // Tamaño de memoria en enteros
int B = 4098;  // Tamaño de bloque en enteros
int K;        // Aridad máxima para merge: M / B - 1

// Leer hasta 'cantidad' enteros desde archivo binario
vector<int> leerEnteros(ifstream& in, int cantidad) {
    vector<int> buffer(cantidad);
    in.read(reinterpret_cast<char*>(buffer.data()), cantidad * sizeof(int));
    buffer.resize(in.gcount() / sizeof(int)); // ajustar si leyó menos
    return buffer;
}

// Escribir enteros a archivo binario
void escribirEnteros(ofstream& out, const vector<int>& datos) {
    out.write(reinterpret_cast<const char*>(datos.data()), datos.size() * sizeof(int));
}

// Paso 1: Crear runs iniciales de tamaño M
vector<string> crearRunsIniciales(const string& archivoEntrada) {
    ifstream in(archivoEntrada, ios::binary);
    vector<string> nombresRuns;
    int runId = 0;

    while (!in.eof()) {
        vector<int> buffer = leerEnteros(in, M);
        if (buffer.empty()) break;
        sort(buffer.begin(), buffer.end());

        string nombreRun = "run_" + to_string(runId++) + ".bin";
        ofstream out(nombreRun, ios::binary);
        escribirEnteros(out, buffer);
        out.close();

        nombresRuns.push_back(nombreRun);
    }

    in.close();
    return nombresRuns;
}

// Paso 2: Fusionar K archivos ordenados en uno
void fusionarKArchivos(const vector<string>& archivos, const string& archivoSalida) {
    int k = archivos.size();
    vector<ifstream> inputs(k);
    vector<vector<int>> buffers(k);
    vector<int> indices(k, 0);
    ofstream out(archivoSalida, ios::binary);
    vector<int> bufferOut;

    // Abrir y cargar buffers iniciales
    for (int i = 0; i < k; ++i) {
        inputs[i].open(archivos[i], ios::binary);
        buffers[i] = leerEnteros(inputs[i], B);
    }

    // Cola de prioridad (min-heap): (valor, índice_archivo)
    auto cmp = [](pair<int, int> a, pair<int, int> b) { return a.first > b.first; };
    priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(cmp)> pq(cmp);

    for (int i = 0; i < k; ++i) {
        if (indices[i] < buffers[i].size())
            pq.push({buffers[i][indices[i]++], i});
    }

    while (!pq.empty()) {
        auto [valor, i] = pq.top(); pq.pop();
        bufferOut.push_back(valor);

        if (bufferOut.size() == B) {
            escribirEnteros(out, bufferOut);
            bufferOut.clear();
        }

        if (indices[i] == buffers[i].size()) {
            buffers[i] = leerEnteros(inputs[i], B);
            indices[i] = 0;
        }

        if (!buffers[i].empty()) {
            pq.push({buffers[i][indices[i]++], i});
        }
    }

    if (!bufferOut.empty()) {
        escribirEnteros(out, bufferOut);
    }

    // Cerrar archivos
    for (auto& f : inputs) f.close();
    out.close();
}

// Paso 3: MergeSort externo completo
void mergeSortExterno(const string& archivoEntrada, const string& archivoSalida) {
    vector<string> runs = crearRunsIniciales(archivoEntrada);
    int nivel = 0;

    while (runs.size() > 1) {
        vector<string> nuevosRuns;
        for (size_t i = 0; i < runs.size(); i += K) {
            vector<string> grupo;
            for (int j = 0; j < K && i + j < runs.size(); ++j)
                grupo.push_back(runs[i + j]);

            string nuevoRun = "nivel" + to_string(nivel) + "_run_" + to_string(nuevosRuns.size()) + ".bin";
            fusionarKArchivos(grupo, nuevoRun);
            nuevosRuns.push_back(nuevoRun);
        }

        // Borrar runs anteriores
        for (const string& r : runs)
            remove(r.c_str());

        runs = nuevosRuns;
        nivel++;
    }

    rename(runs[0].c_str(), archivoSalida.c_str());
}

// Utilidad para crear un archivo binario de prueba
void generarArchivoAleatorio(const string& nombre, int n) {
    ofstream out(nombre, ios::binary);
    for (int i = 0; i < n; ++i) {
        int valor = rand() % 10000;
        out.write(reinterpret_cast<char*>(&valor), sizeof(int));
    }
    out.close();
}

// Utilidad para imprimir archivo binario
void imprimirArchivo(const string& nombre) {
    ifstream in(nombre, ios::binary);
    int valor;
    while (in.read(reinterpret_cast<char*>(&valor), sizeof(int))) {
        cout << valor << " ";
    }
    cout << endl;
    in.close();
}

// main para pruebas
int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Uso: ./mergesort <archivo_entrada> <archivo_salida> <n_elementos>" << endl;
        return 1;
    }

    string archivoEntrada = argv[1];
    string archivoSalida = argv[2];
    int n = stoi(argv[3]);

    // Calcular K
    K = M / B - 1;

    // generarArchivoAleatorio(archivoEntrada, n); // Solo si necesitas generar datos de prueba

    mergeSortExterno(archivoEntrada, archivoSalida);

    cout << "Archivo ordenado generado: " << archivoSalida << endl;
    // imprimirArchivo(archivoSalida); // Descomenta si quieres ver los resultados

    return 0;
}
