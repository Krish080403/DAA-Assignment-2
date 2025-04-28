#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <limits>
#include <fstream>
#include <string>

using namespace std;

class Graph {
public:
    int n;
    int m;
    vector<vector<int>> adj;
    
    Graph(int n) : n(n), m(0) {
        adj.resize(n);
    }
    
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        m++;
    }
    
    Graph induceSubgraph(const vector<int>& vertices) const {
        unordered_map<int, int> vertexMap;
        for (int i = 0; i < vertices.size(); i++) {
            vertexMap[vertices[i]] = i;
        }
        
        Graph subgraph(vertices.size());
        for (int i = 0; i < vertices.size(); i++) {
            int u = vertices[i];
            for (int v : adj[u]) {
                auto it = vertexMap.find(v);
                if (it != vertexMap.end() && it->second > i) {
                    subgraph.addEdge(i, it->second);
                }
            }
        }
        
        return subgraph;
    }
    
    int degree(int v) const {
        return adj[v].size();
    }
    
    int maxDegree() const {
        int maxDeg = 0;
        for (int v = 0; v < n; v++) {
            maxDeg = max(maxDeg, degree(v));
        }
        return maxDeg;
    }
    
    double density() const {
        if (n <= 1) return 0;
        return static_cast<double>(m) / n;
    }
};

class FlowNetwork {
public:
    int n;
    vector<vector<double>> capacity;
    vector<vector<int>> adj;
    
    FlowNetwork(int n) : n(n) {
        capacity.resize(n, vector<double>(n, 0.0));
        adj.resize(n);
    }
    
    void addEdge(int u, int v, double cap) {
        if (capacity[u][v] == 0) {
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
        capacity[u][v] += cap;
    }
    
    double maxFlow(int s, int t) {
        vector<vector<double>> flow(n, vector<double>(n, 0.0));
        double totalFlow = 0.0;
        
        while (true) {
            vector<int> parent(n, -1);
            queue<int> q;
            q.push(s);
            parent[s] = -2;
            
            while (!q.empty() && parent[t] == -1) {
                int u = q.front();
                q.pop();
                
                for (int v : adj[u]) {
                    if (parent[v] == -1 && capacity[u][v] > flow[u][v]) {
                        parent[v] = u;
                        q.push(v);
                    }
                }
            }
            
            if (parent[t] == -1) break;
            
            double pathFlow = numeric_limits<double>::max();
            for (int v = t; v != s; v = parent[v]) {
                int u = parent[v];
                pathFlow = min(pathFlow, capacity[u][v] - flow[u][v]);
            }
            
            for (int v = t; v != s; v = parent[v]) {
                int u = parent[v];
                flow[u][v] += pathFlow;
                flow[v][u] -= pathFlow;
            }
            
            totalFlow += pathFlow;
        }
        
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(s);
        visited[s] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int v : adj[u]) {
                if (!visited[v] && capacity[u][v] > flow[u][v]) {
                    visited[v] = true;
                    q.push(v);
                }
            }
        }
        
        minCutVertices.clear();
        for (int v = 0; v < n; v++) {
            if (visited[v] && v != s) {
                minCutVertices.push_back(v);
            }
        }
        
        return totalFlow;
    }
    
    vector<int> minCutVertices;
};

vector<int> computeKCore(const Graph& g, int k) {
    vector<int> degrees(g.n);
    for (int v = 0; v < g.n; v++) {
        degrees[v] = g.degree(v);
    }
    
    vector<bool> deleted(g.n, false);
    queue<int> q;
    
    for (int v = 0; v < g.n; v++) {
        if (degrees[v] < k) {
            q.push(v);
            deleted[v] = true;
        }
    }
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        
        for (int v : g.adj[u]) {
            if (!deleted[v]) {
                degrees[v]--;
                if (degrees[v] < k) {
                    q.push(v);
                    deleted[v] = true;
                }
            }
        }
    }
    
    vector<int> kCore;
    for (int v = 0; v < g.n; v++) {
        if (!deleted[v]) {
            kCore.push_back(v);
        }
    }
    
    return kCore;
}

vector<int> coreExact(const Graph& g) {
    int n = g.n;
    
    double l = 0.0;
    double u = g.maxDegree();
    
    vector<int> densestSubgraph;
    double maxDensity = 0.0;
    
    int maxK = g.maxDegree();
    
    for (int k = 1; k <= maxK; k++) {
        vector<int> kCore = computeKCore(g, k);
        if (kCore.empty()) continue;
        
        Graph subgraph = g.induceSubgraph(kCore);
        int subN = subgraph.n;
        int subM = subgraph.m;
        
        double lk = l;
        double uk = u;
        double eps = 1.0 / (n * (n - 1));
        
        while (uk - lk > eps) {
            double alpha = (lk + uk) / 2.0;
            
            FlowNetwork network(subN + 2);
            int s = subN;
            int t = subN + 1;
            
            for (int v = 0; v < subN; v++) {
                network.addEdge(s, v, subM);
                network.addEdge(v, t, subM + 2 * alpha - subgraph.degree(v));
            }
            
            for (int u = 0; u < subN; u++) {
                for (int v : subgraph.adj[u]) {
                    if (v > u) {
                        network.addEdge(u, v, 1.0);
                        network.addEdge(v, u, 1.0);
                    }
                }
            }
            
            network.maxFlow(s, t);
            
            if (!network.minCutVertices.empty()) {
                vector<int> cutOriginal;
                for (int v : network.minCutVertices) {
                    cutOriginal.push_back(kCore[v]);
                }
                
                Graph denseSubgraph = g.induceSubgraph(cutOriginal);
                double density = denseSubgraph.density();
                
                if (density > maxDensity) {
                    maxDensity = density;
                    densestSubgraph = cutOriginal;
                    l = density;
                }
                
                lk = alpha;
            } else {
                uk = alpha;
            }
        }
    }
    
    return densestSubgraph;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <graph_file>" << endl;
        return 1;
    }
    
    string filename = argv[1];
    cout << "Reading graph from " << filename << "..." << endl;
    
    ifstream file(filename);
    if (!file) {
        cerr << "Error: Cannot open file " << filename << endl;
        return 1;
    }
    
    int maxNode = -1;
    vector<pair<int, int>> edges;
    int u, v;
    
    while (file >> u >> v) {
        maxNode = max(maxNode, max(u, v));
        edges.push_back({u, v});
    }
    
    int n = maxNode + 1;
    Graph g(n);
    
    for (const auto& edge : edges) {
        g.addEdge(edge.first, edge.second);
    }
    
    cout << "Graph has " << g.n << " vertices and " << g.m << " edges." << endl;
    cout << "Running CoreExact algorithm..." << endl;
    
    auto start = clock();
    vector<int> densestSubgraph = coreExact(g);
    auto end = clock();
    
    Graph denseSubgraph = g.induceSubgraph(densestSubgraph);
    
    cout << "Densest subgraph found:" << endl;
    cout << "Number of vertices: " << densestSubgraph.size() << endl;
    cout << "Density: " << denseSubgraph.density() << endl;
    cout << "Time taken: " << (double)(end - start) / CLOCKS_PER_SEC << " seconds" << endl;
    
    cout << "Vertices in densest subgraph: ";
    for (size_t i = 0; i < min(densestSubgraph.size(), size_t(20)); i++) {
        cout << densestSubgraph[i] << " ";
    }
    if (densestSubgraph.size() > 20) cout << "... (and " << densestSubgraph.size()-20 << " more)";
    cout << endl;
    
    return 0;
}
