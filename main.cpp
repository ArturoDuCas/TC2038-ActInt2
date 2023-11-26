/*
    Actividad Integradora 2 - TC2038
    Integrantes:
        Arturo Duran Castillo - A00833516
        Andrés Marcelo De Luna Pámanes - A00832239
    Fecha: 26 de noviembre 2023
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

struct Colonia {
    string nombre;
    int x, y, central;
};

struct Conexion {
    string colonia1, colonia2;
    int costo;
};

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
    int n, m, central, k;
    cin >> n >> m >> central >> k;

    vector<Colonia> colonias(n);
    vector<Conexion> conexiones(m);

    ConjuntoDisjunto conjDisjunto;

    for (int i = 0; i < n; ++i) {
        cin >> colonias[i].nombre >> colonias[i].x >> colonias[i].y >> colonias[i].central;
        // Assign index to colony names
        conjDisjunto.indiceColonias[colonias[i].nombre] = colonias[i].nombre;
    }

    for (int i = 0; i < m; ++i) {
        cin >> conexiones[i].colonia1 >> conexiones[i].colonia2 >> conexiones[i].costo;
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