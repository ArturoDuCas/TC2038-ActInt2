/*
    Actividad Integradora 2 - TC2038
    Integrantes:
        Arturo Duran Castillo - A00833516
        Andrés Marcelo De Luna Pámanes - A00832239
    Fecha: 26 de noviembre 2023
 */

#include <iostream>
#include <climits>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Colonia {
    string name;
    int x, y, isCentral;
};

struct Conexion {
    string colonia1, colonia2;
    int costo;
};

void printVecOfVecOfInt(vector<vector<int>> &vec) {
    for(int i = 0; i < vec.size(); i++) {
        for(int j = 0; j < vec.size(); j++) {
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
}

struct ConjuntoDisjunto {
    map<string, string> indiceColonias;

    string encontrar(string u) {
        if (indiceColonias[u] != u) {
            indiceColonias[u] = encontrar(indiceColonias[u]);
        }
        return indiceColonias[u];
    }

    void unir(string u, string v) {
        string pu = encontrar(u);
        string pv = encontrar(v);

        indiceColonias[pu] = pv;
    }
};

bool compararConexiones(const Conexion &a, const Conexion &b) {
    return a.costo < b.costo;
}

vector<Conexion> encontrarMST(vector<Conexion> &conexiones, int n, ConjuntoDisjunto &conjDisjunto) {
    sort(conexiones.begin(), conexiones.end(), compararConexiones);

    vector<Conexion> mst;

    for (const Conexion &conexion : conexiones) {
        string u = conexion.colonia1;
        string v = conexion.colonia2;

        if (conjDisjunto.encontrar(u) != conjDisjunto.encontrar(v)) {
            mst.push_back(conexion);
            conjDisjunto.unir(u, v);
        }
    }

    return mst;
}

int main() {
    // n = number of neighborhoods
    // m = number of connections
    // k = connections with new cable
    // q = number of new neighborhoods
    int n, m, k, q;
    cin >> n >> m >> k >> q;

    vector<Colonia> colonias(n);
    vector<Conexion> conexiones(m);
    // TODO: verify if they are always int values
    vector<vector<int>> matAdj(n, vector<int>(n, INT_MAX)); // for tsp
    unordered_map<string, int> neighborhoodsIndex;


    ConjuntoDisjunto conjDisjunto;

    // Read neighborhoods
    for (int i = 0; i < n; ++i) {
        cin >> colonias[i].name >> colonias[i].x >> colonias[i].y >> colonias[i].isCentral;
        neighborhoodsIndex[colonias[i].name] = i;
        // Assign index to colony names
        conjDisjunto.indiceColonias[colonias[i].name] = colonias[i].name;
    }

    // Read connections
    for (int i = 0; i < m; ++i) {
        cin >> conexiones[i].colonia1 >> conexiones[i].colonia2 >> conexiones[i].costo;

        // Add edge to adjacency matrix
        int u = neighborhoodsIndex[conexiones[i].colonia1];
        int v = neighborhoodsIndex[conexiones[i].colonia2];
        matAdj[u][v] = matAdj[v][u] = conexiones[i].costo;
    }

    vector<Conexion> mst = encontrarMST(conexiones, n, conjDisjunto);

    cout << "-------------------" << endl;
    cout << "1 - Cableado óptimo de nueva conexión." << endl;

    int costoTotal = 0;
    for (const Conexion &conexion : mst) {
        cout << conexion.colonia1 << " - " << conexion.colonia2 << " " << conexion.costo << endl;
        costoTotal += conexion.costo;
    }

    cout << "Costo Total: " << costoTotal << endl;
    cout << "-------------------" << endl;

    return 0;
}