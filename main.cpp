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
#include <unordered_map>
#include <cmath>
#include <cfloat>

using namespace std;

struct Colonia {
    string name;
    int x, y, isCentral;
};

struct Conexion {
    string colonia1, colonia2;
    int costo;
    bool isNew = false;
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

/**
 * @title Euclidean Distance Calculator
 *
 * @description
 * Calculates the Euclidean distance between two neighborhoods represented by their coordinates.
 *
 * @param b1 : const Colonia&
 *        The first neighborhood, represented by a struct with name, x-coordinate, y-coordinate, and centrality indicator.
 *
 * @param b2 : const Colonia&
 *        The second neighborhood, represented by a struct with name, x-coordinate, y-coordinate, and centrality indicator.
 *
 * @return double
 *         Returns the Euclidean distance between the two neighborhoods.
 *
 * @complexity O(1)
 *         The function has constant time complexity as it involves simple arithmetic operations.
 */
double dist(const Colonia &b1, const Colonia &b2) {
    return sqrt((b1.x - b2.x) * (b1.x - b2.x) + (b1.y - b2.y) * (b1.y - b2.y));
}

// Print functions
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

void printUnorderedMap(unordered_map<int, int> &map) {
    for(auto it = map.begin(); it != map.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }
}

// Helper functions
bool isCentralNeighborhood(unordered_set<int> &centralNeighborhoodsSet, int i) {
    return centralNeighborhoodsSet.find(i) != centralNeighborhoodsSet.end();
}

bool compararConexiones(const Conexion &a, const Conexion &b) {
    return a.costo < b.costo;
}


/**
 * @title Minimum Spanning Tree (MST) Finder
 *
 * @description
 * Finds the Minimum Spanning Tree (MST) for a given set of connections using Kruskal's algorithm.
 * The function sorts the connections based on their costs and selects edges in ascending order while
 * avoiding cycles to construct the MST.
 *
 * @param conexiones : vector<Conexion>&
 *        The vector of connections, each represented by the source, destination, and cost.
 *
 * @param n : int
 *        The total number of neighborhoods in the graph.
 *
 * @param conjDisjunto : ConjuntoDisjunto&
 *        The disjoint-set data structure to keep track of connected components and avoid cycles.
 *
 * @return vector<Conexion>
 *         Returns the Minimum Spanning Tree (MST) as a vector of connections.
 *
 * @complexity O(E * log(E) + α(N))
 *         E is the number of edges (connections), log(E) is the complexity of the sort operation,
 *         and α(N) is the amortized complexity of the disjoint-set union and find operations.
 */
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


/**
 * @title Calculate Possible Cost for TSP Node
 *
 * @description
 * Estimates the potential total cost of a given TSP node by considering its accumulated cost and the
 * minimal possible cost to reach any remaining unvisited node. This function is essential in the
 * branch-and-bound approach of the TSP solution, providing a heuristic to prioritize nodes in the search.
 *
 * @param n : tsp_node&
 *        Reference to the current TSP node whose potential cost is being calculated. The node contains
 *        information about the current state of the route, including visited nodes and accumulated cost.
 *
 * @param mat : vector<vector<int>>&
 *        The matrix representing the cost (distance) between each pair of nodes in the graph.
 *
 * @return void
 *         The function does not return a value but updates the potential cost (`posCost`) of the tsp_node `n`.
 *
 * @complexity O(n^2)
 *
 */
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


string showNeighborhoodName(unordered_map<string, int> &neighborhoodsDict, int i) {
    for(auto it = neighborhoodsDict.begin(); it != neighborhoodsDict.end(); it++) {
        if(it->second == i) {
            return it->first;
        }
    }

    return "";
}


/**
 * @title Recursive Helper for Displaying a Route
 *
 * @description
 * Recursively traverses and displays the shortest path between two points using the Floyd-Warshall path matrix.
 *
 * @param fwPath : vector<vector<int>>&
 *        The path matrix generated by the Floyd-Warshall algorithm, indicating the next node in the shortest
 *        path for each pair of nodes.
 *
 * @param x : int
 *        The starting node index of the current path segment being displayed.
 *
 * @param y : int
 *        The ending node index of the current path segment being displayed.
 *
 * @param neighborhoodsDict : unordered_map<string, int>&
 *        A dictionary mapping neighborhood names to their indices, used for converting node indices to
 *        neighborhood names for display.
 *
 * @return void
 *         The function does not return a value but prints the path segment directly to the console.
 *
 * @complexity O(n)
 */
void showRouteHelper(vector<vector<int>> &fwPath, int x, int y, unordered_map<string, int> &neighborhoodsDict) {
    if (fwPath[x][y] == -1) {
        cout << " - " << showNeighborhoodName(neighborhoodsDict, y);
        return;
    }
    showRouteHelper(fwPath, x, fwPath[x][y], neighborhoodsDict);
    showRouteHelper(fwPath, fwPath[x][y], y, neighborhoodsDict);

}


void showRoute(vector<int> &route, unordered_map<string, int> &neighborhoodsDict, vector<vector<int>> &fwPath, unordered_map<int, int> &originalIndex) {
    cout << endl;

    cout << showNeighborhoodName(neighborhoodsDict, originalIndex[route[0]]);
    for(int i = 0; i < route.size() - 1; i++) {
        showRouteHelper(fwPath, originalIndex[route[i]], originalIndex[route[i+1]], neighborhoodsDict);
    }

    cout << endl << endl;
}


/**
 * @title Floyd-Warshall Algorithm for Shortest Paths
 *
 * @description
 * Implements the Floyd-Warshall algorithm to find the shortest paths between all pairs of nodes
 * in a weighted graph.
 *
 * @param mat : vector<vector<int>>&
 *        A matrix representing the distances between each pair of nodes in the graph.
 *
 * @param n : int
 *        The number of nodes in the graph.
 *
 * @param fwPath : vector<vector<int>>&
 *        A matrix for storing the next node in the shortest path between each pair of nodes.
 *
 * @return vector<vector<int>>
 *         Returns the matrix containing the shortest distances between each pair of nodes.
 *
 * @complexity O(n^3)
 */
vector<vector<int>> floydWarshall(vector<vector<int>> mat, int n, vector<vector<int>> &fwPath) {
    for(int k = 0; k <n; k++) {
        mat[k][k] = 0;
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                if(mat[i][k] != INT_MAX && mat[k][j] != INT_MAX &&
                   mat[i][k]+mat[k][j] < mat[i][j]) {
                    mat[i][j] = mat[i][k]+mat[k][j];
                    fwPath[i][j] = k;
                }
            }
        }
    }

    return mat;
}

vector<vector<int>> createNonCentralMat(vector<vector<int>> mat, int n, unordered_set<int> &centralNeighborhoodsSet, unordered_map<int, int> &originalIndex) {
    vector<vector<int>> nonCentralMat;

    for (int i = 0; i < n; i++) {
        if (!isCentralNeighborhood(centralNeighborhoodsSet, i)) {
            vector<int> row;
            for (int j = 0; j < n; j++) {
                if (!isCentralNeighborhood(centralNeighborhoodsSet, j)) {
                    row.push_back(mat[i][j]);
                }
            }
            originalIndex[nonCentralMat.size()] = i;
            nonCentralMat.push_back(row);
            nonCentralMat[nonCentralMat.size()-1][nonCentralMat.size()-1] = INT_MAX;

        }
    }

    return nonCentralMat;
}


/**
 * @title Traveling Salesman Problem (TSP) Solver
 *
 * @description
 * Solves the Traveling Salesman Problem (TSP) for a set of nodes (neighborhoods) using a branch-and-bound approach.
 * The function aims to find the shortest possible route that visits each non-central neighborhood exactly once
 * and returns to the starting neighborhood. The solution considers the possibility of passing through central
 * neighborhoods as intermediate points.
 *
 * @param mat : vector<vector<int>>&
 *        The adjacency matrix representing the distances between each pair of neighborhoods.
 *
 * @param n : int
 *        The total number of neighborhoods in the graph.
 *
 * @param centralNeighborhoodsSet : unordered_set<int>&
 *        A set containing the indices of the central neighborhoods, which are not included in the TSP route but
 *        can be passed through.
 *
 * @param neighborhoodsDict : unordered_map<string, int>&
 *        A dictionary mapping neighborhood names to their indices, used for displaying the names of neighborhoods.
 *
 * @param fwPath : vector<vector<int>>&
 *        The path matrix generated by the Floyd-Warshall algorithm, used for determining the shortest paths
 *        between neighborhoods.
 *
 * @return void
 *         The function does not return a value but prints the optimal route and its cost to the console.
 *
 * @complexity O(n^2 * 2^n)
 */
void tsp(vector<vector<int>> &mat, int n, unordered_set<int> &centralNeighborhoodsSet, unordered_map<string, int> &neighborhoodsDict, vector<vector<int>> &fwPath) {
    vector<vector<int>> fwMat =  floydWarshall(mat, n, fwPath);
    unordered_map<int, int> originalIndex; // index of non-central neighborhoods in original matrix
    vector<vector<int>> nonCentralMat = createNonCentralMat(fwMat, n, centralNeighborhoodsSet, originalIndex);
    n = nonCentralMat.size();

    priority_queue<tsp_node> pq;
    tsp_node initialNode;
    initialNode.level = 0;
    initialNode.parent = -1;
    initialNode.actual = 0;
    initialNode.visited.resize(n, false);
    initialNode.visited[0] = true;
    initialNode.route.push_back(0);
    initialNode.acumCost = 0;
    calculatePossibleCost(initialNode, nonCentralMat);
    pq.push(initialNode);

    int optCost = INT_MAX;
    vector<int> optRoute;
    while(!pq.empty()) {
        tsp_node actual = pq.top();
        pq.pop();

        if(actual.posCost > optCost) {
            continue;
        } else if(actual.level == n-1) {
            if(nonCentralMat[actual.actual][0] != INT_MAX) {
                if (actual.acumCost + nonCentralMat[actual.actual][0] < optCost) {
                    optCost = actual.acumCost + nonCentralMat[actual.actual][0];
                    optRoute = actual.route;
                    optRoute.push_back(0);
                }
            }
            continue;
        } else {
            for(int i = 0; i < n; i++) {
                if(nonCentralMat[actual.actual][i] != INT_MAX && !actual.visited[i]) {
                    tsp_node child;
                    child.level = actual.level + 1;
                    child.parent = actual.actual;
                    child.actual = i;
                    child.visited = actual.visited;
                    child.visited[i] = true;
                    child.route = actual.route;
                    child.route.push_back(i);
                    child.acumCost = actual.acumCost + nonCentralMat[actual.actual][i];
                    calculatePossibleCost(child, nonCentralMat);
                    pq.push(child);
                }
            }
        }
    }


    showRoute(optRoute, neighborhoodsDict, fwPath, originalIndex);
    cout << "La Ruta Óptima tiene un costo total de: " << optCost << endl;

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
    vector<vector<int>> matAdj(n, vector<int>(n, INT_MAX));
    vector<vector<int>> fwPath(n, vector<int>(n)); // -1 means direct connection
    unordered_map<string, int> neighborhoodsDict;
    unordered_set<int> centralNeighborhoodsSet;


    ConjuntoDisjunto conjDisjunto;

    // Read neighborhoods
    for (int i = 0; i < n; ++i) {
        cin >> colonias[i].name >> colonias[i].x >> colonias[i].y >> colonias[i].isCentral;
        neighborhoodsDict[colonias[i].name] = i;

        // Assign index to colony names
        conjDisjunto.indiceColonias[colonias[i].name] = colonias[i].name;

        // Add central neighborhoods to set
        if (colonias[i].isCentral) {
            centralNeighborhoodsSet.insert(i);
        }

        // Add direct connections to path matrix
        fwPath[i][i] = -1;
    }

    // Read connections
    for (int i = 0; i < m; ++i) {
        cin >> conexiones[i].colonia1 >> conexiones[i].colonia2 >> conexiones[i].costo;

        // Add edge to adjacency matrix
        int u = neighborhoodsDict[conexiones[i].colonia1];
        int v = neighborhoodsDict[conexiones[i].colonia2];
        matAdj[u][v] = matAdj[v][u] = conexiones[i].costo;

        // Add edge to path matrix
        fwPath[u][v] = fwPath[v][u] = -1;
    }

    // Read connections with new cable
    for (int i = 0; i < k; i++) {
        string c1, c2;
        cin >> c1 >> c2;

        for(int j = 0; j < conexiones.size(); j++) {
            if((conexiones[j].colonia1 == c1 && conexiones[j].colonia2 == c2) || (conexiones[j].colonia1 == c2 && conexiones[j].colonia2 == c1)) {
                conexiones[j].isNew = true;
                break;
            }
        }
    }

    vector<Conexion> mst = encontrarMST(conexiones, n, conjDisjunto);

    cout << "-------------------" << endl;
    cout << "1 - Cableado óptimo de nueva conexión." << endl << endl;

    int costoTotal = 0;
    for(int i = 0; i < mst.size(); i++) {
        Conexion c = mst[i];
        if (c.costo > 0 && !c.isNew) {
            cout << c.colonia1 << " - " << c.colonia2 << " " << c.costo << endl;
            costoTotal += c.costo;
        }
    }

    cout << endl << "Costo Total: " << costoTotal << endl;
    cout << "-------------------" << endl;


    // 2. FLoyd-Warshall + TSP
    cout << "2 – La ruta óptima." << endl;
    tsp(matAdj, n, centralNeighborhoodsSet, neighborhoodsDict, fwPath);
    cout << "-------------------" << endl;

   // 3. Most optimal path between centrals
    cout << "3 – Caminos más cortos entre centrales." << endl;

    vector<vector<int>> fwCentralPath = floydWarshall(matAdj, n, fwPath);

    for (int i = 0; i < n; ++i) {
        if (colonias[i].isCentral) {
            for (int j = i + 1; j < n; ++j) {
                if (colonias[j].isCentral) {
                    cout << showNeighborhoodName(neighborhoodsDict, i);
                    showRouteHelper(fwPath, i, j, neighborhoodsDict);
                    cout << " (" << fwCentralPath[i][j] << ")" << endl;
                }
            }
        }
    }

    cout << "-------------------" << endl;
    
    // Read new colonies
    vector<Colonia> nuevasColonias(q);
    for (int i = 0; i < q; ++i) {
        cin >> nuevasColonias[i].name >> nuevasColonias[i].x >> nuevasColonias[i].y;
    }


    cout << "4 – Conexión de nuevas colonias." << endl;

    for (int i = 0; i < q; ++i) {
        int minDist = INT_MAX;
        string coloniaExistente;

        // Find the closest existing colony for each new colony
        for (const Colonia &colonia : colonias) {
            double distance = dist(colonia, nuevasColonias[i]);
            if (distance < minDist) {
                minDist = distance;
                coloniaExistente = colonia.name;
            }
        }

        cout << nuevasColonias[i].name << " debe conectarse con " << coloniaExistente << endl;
    }

    cout << "-------------------" << endl;

    return 0;
}