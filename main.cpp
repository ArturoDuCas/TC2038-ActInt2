/*
    Actividad Integradora 2 - TC2038
    Integrantes:
        Arturo Duran Castillo - A00833516
        Andrés Marcelo De Luna Pámanes - A00832239
    Fecha: 26 de noviembre 2023
 */

#include <iostream>
#include <climits>
#include <unordered_set>
#include <queue>
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

void printVecOfInt(vector<int> &vec) {
    for(int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void printVecOfVecOfInt(vector<vector<int>> &vec) {
    for(int i = 0; i < vec.size(); i++) {
        for(int j = 0; j < vec.size(); j++) {
            cout << vec[i][j] << " ";
        }
        cout << endl;
    }
}

void printVecOfBool(vector<bool> &vec) {
    for(int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void printUnorderedSet(unordered_set<int> &set) {
    for(int i : set) {
        cout << i << " ";
    }
    cout << endl;
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


struct tsp_node {
    int level;
    int acumCost;
    int posCost;
    int parent;
    int actual;
    vector<bool> visited;
    vector<int> route;

    bool operator<(const tsp_node &other) const { // for priority queue
        return posCost >= other.posCost;
    }
};

void calculatePossibleCost(tsp_node &n, vector<vector<int>> mat) {
    n.posCost = n.acumCost;
    int obtainedCost;

    for (int i = 0; i < mat.size(); i++) { // for each node
        obtainedCost = INT_MAX;
        if (!n.visited[i] || i == n.actual) { // if node is not visited or is the actual node
            if (!n.visited[i]) { // if node is not visited
                for (int j = 0; j < mat.size(); j++) {
                    if (mat[i][j] < obtainedCost && i != j) {
                        obtainedCost = mat[i][j];
                    }
                }
            }
            if (obtainedCost != INT_MAX) { // if there is a possible cost
                n.posCost += obtainedCost;
            }
        }
    }
}

void showRoute(vector<int> &route, unordered_map<string, int> &neighborhoodsIndex) {
    cout << endl;
    string initialNeighborhood;
    for(int i = 0; i < route.size(); i++) {
        for(auto it = neighborhoodsIndex.begin(); it != neighborhoodsIndex.end(); it++) {
            if(it->second == route[i]) {
                cout << it->first << " - ";
                if(i == 0) {
                    initialNeighborhood = it->first;
                }
            }
        }
    }

    cout << initialNeighborhood;


    cout << endl;
}

void tsp(vector<vector<int>> &mat, int n, unordered_set<int> &centralNeighborhoods, unordered_map<string, int> &neighborhoodsIndex) {
    // find initial neighborhood
    int initialNeighborhood;
    for (int i = 0; i < n; i++) {
        if (centralNeighborhoods.find(i) == centralNeighborhoods.end()) {
            initialNeighborhood = i;
            break;
        }
    }

    priority_queue<tsp_node> pq;
    tsp_node initialNode;
    initialNode.level = 0;
    initialNode.parent = -1;
    initialNode.actual = initialNeighborhood;
    initialNode.visited.resize(n, false);
    initialNode.visited[initialNeighborhood] = true;
    initialNode.route.push_back(initialNeighborhood);
    initialNode.acumCost = 0;
    calculatePossibleCost(initialNode, mat);
    pq.push(initialNode);


    int optCost = INT_MAX;
    vector<int> optRoute;
    while(!pq.empty()) {
        tsp_node actual = pq.top();
        pq.pop();


        if(actual.posCost > optCost) {
            continue;
        } else if(actual.level == n-1) {
            if(mat[actual.actual][initialNeighborhood] != INT_MAX) {
                optCost = min(optCost, actual.acumCost + mat[actual.actual][initialNeighborhood]);
                optRoute = actual.route;
            }
            continue;
        } else {
            for(int i = 0; i < n; i++) {
                if(mat[actual.actual][i] != INT_MAX && !actual.visited[i]) {
                    tsp_node child;
                    child.level = actual.level + 1;
                    child.parent = actual.actual;
                    child.actual = i;
                    child.visited = actual.visited;
                    child.visited[i] = true;
                    child.route = actual.route;
                    child.route.push_back(i);
                    child.acumCost = actual.acumCost + mat[actual.actual][i];
                    calculatePossibleCost(child, mat);
                    pq.push(child);
                }
            }
        }
    }


    showRoute(optRoute, neighborhoodsIndex);
    cout << "Costo óptimo: " << optCost << endl;

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
    vector<vector<int>> matAdj(n, vector<int>(n, INT_MAX));
    unordered_map<string, int> neighborhoodsIndex;
    unordered_set<int> centralNeighborhoods;


    ConjuntoDisjunto conjDisjunto;

    // Read neighborhoods
    for (int i = 0; i < n; ++i) {
        cin >> colonias[i].name >> colonias[i].x >> colonias[i].y >> colonias[i].isCentral;
        neighborhoodsIndex[colonias[i].name] = i;

        // Assign index to colony names
        conjDisjunto.indiceColonias[colonias[i].name] = colonias[i].name;

        // Add central neighborhoods to set
        if (colonias[i].isCentral) {
            centralNeighborhoods.insert(i);
        }
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



    // 2. TSP
    cout << "2 – La ruta óptima." << endl;
    tsp(matAdj, n, centralNeighborhoods, neighborhoodsIndex);


    return 0;
}