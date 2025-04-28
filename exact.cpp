#include <bits/stdc++.h>
using namespace std;

// Dinic's Algorithm
struct Edge {
    int to, rev;
    double cap;
};

class MaxFlow {
public:
    vector<vector<Edge>> g;
    vector<int> level, iter;
    int n;

    MaxFlow(int n) : n(n) {
        g.resize(n);
        level.resize(n);
        iter.resize(n);
    }

    void add_edge(int from, int to, double cap) {
        g[from].push_back({to, (int)g[to].size(), cap});
        g[to].push_back({from, (int)g[from].size()-1, 0});
    }

    void bfs(int s) {
        fill(level.begin(), level.end(), -1);
        queue<int> q;
        level[s] = 0;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (auto &e : g[v]) {
                if (e.cap > 1e-9 && level[e.to] < 0) {
                    level[e.to] = level[v] + 1;
                    q.push(e.to);
                }
            }
        }
    }

    double dfs(int v, int t, double upTo) {
        if (v == t) return upTo;
        for (int &i = iter[v]; i < g[v].size(); i++) {
            Edge &e = g[v][i];
            if (e.cap > 1e-9 && level[v] < level[e.to]) {
                double d = dfs(e.to, t, min(upTo, e.cap));
                if (d > 0) {
                    e.cap -= d;
                    g[e.to][e.rev].cap += d;
                    return d;
                }
            }
        }
        return 0;
    }

    double max_flow(int s, int t) {
        double flow = 0;
        while (true) {
            bfs(s);
            if (level[t] < 0) break;
            fill(iter.begin(), iter.end(), 0);
            double f;
            while ((f = dfs(s, t, 1e18)) > 0) {
                flow += f;
            }
        }
        return flow;
    }
};

// Graph structures
map<int, int> nodeIdMap;
vector<pair<int,int>> edges;
vector<vector<int>> adj;
int n, m;

bool readGraph(const string &filename) {
    cout << "Attempting to open file: " << filename << endl;
    
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return false;
    }
    
    cout << "File opened successfully." << endl;
    
    nodeIdMap.clear();
    edges.clear();
    adj.clear();
    int id = 0;
    int u, v;
    int edgeCount = 0;
    
    while (in >> u >> v) {
        edgeCount++;
        if (edgeCount % 1000 == 0) {
            cout << "Read " << edgeCount << " edges so far..." << endl;
        }
        
        if (!nodeIdMap.count(u)) nodeIdMap[u] = id++;
        if (!nodeIdMap.count(v)) nodeIdMap[v] = id++;
        edges.emplace_back(nodeIdMap[u], nodeIdMap[v]);
    }
    
    if (edges.empty()) {
        cerr << "Error: No edges read from file." << endl;
        return false;
    }
    
    n = id;
    m = edges.size();
    adj.resize(n);
    for (auto &[u,v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
    
    cout << "Successfully read " << n << " nodes and " << m << " edges." << endl;
    return true;
}

// Build flow network exactly as per paper and check
bool buildAndCheck(double alpha, vector<int>& subset) {
    int S = 0; // source
    int T = 1; // sink
    int base = 2; // graph nodes from here
    int V = base + n;

    MaxFlow mf(V);

    // Add s->v and v->t
    for (int v = 0; v < n; ++v) {
        mf.add_edge(S, base+v, m);
        mf.add_edge(base+v, T, m + 2*alpha - adj[v].size());
    }

    // For each edge u-v
    for (auto &[u, v] : edges) {
        mf.add_edge(base+u, base+v, 1);
        mf.add_edge(base+v, base+u, 1);
    }

    mf.max_flow(S, T);

    // Find reachable from source
    vector<bool> reachable(V, false);
    queue<int> q;
    q.push(S);
    reachable[S] = true;
    while (!q.empty()) {
        int v = q.front(); q.pop();
        for (auto &e : mf.g[v]) {
            if (e.cap > 1e-9 && !reachable[e.to]) {
                reachable[e.to] = true;
                q.push(e.to);
            }
        }
    }

    subset.clear();
    for (int v = 0; v < n; ++v) {
        if (reachable[base+v]) subset.push_back(v);
    }

    return !subset.empty();
}

// Function to calculate density of the subgraph
double calculateDensity(const vector<int>& subset) {
    // Create a set of nodes in the subset for O(1) lookups
    unordered_set<int> subsetNodes;
    for (int v : subset) {
        subsetNodes.insert(v);
    }
    
    int edgeCount = 0;
    
    // Count edges where both endpoints are in the subset
    for (auto &[u, v] : edges) {
        if (subsetNodes.count(u) && subsetNodes.count(v)) {
            edgeCount++;
        }
    }
    
    // Calculate density as |E|/|V|
    return subset.size() > 0 ? (double)edgeCount / subset.size() : 0;
}

bool solve(const string &filename) {
    if (!readGraph(filename)) {
        return false;
    }

    double low = 0, high = 0;
    for (int v = 0; v < n; ++v) {
        high = max(high, (double)adj[v].size());
    }
    
    cout << "Starting binary search with high = " << high << endl;

    vector<int> bestSubset;

    while (high - low >= 1.0 / (n*(n-1))) {
        double mid = (low + high) / 2.0;
        vector<int> subset;
        cout << "Trying alpha = " << mid << "..." << endl;
        if (buildAndCheck(mid, subset)) {
            low = mid;
            bestSubset = subset;
            cout << "  Found valid subset of size " << subset.size() << endl;
        } else {
            high = mid;
            cout << "  No valid subset found" << endl;
        }
    }

    // Calculate the density of the best subset
    double density = calculateDensity(bestSubset);
    
    cout << "\n==== RESULTS ====\n";
    cout << "DENSITY: " << fixed << setprecision(6) << density << endl;
    cout << "Subset size: " << bestSubset.size() << " nodes" << endl;
    cout << "Nodes in densest subgraph: ";
    for (int v : bestSubset) {
        for (auto &[realId, mappedId] : nodeIdMap) {
            if (mappedId == v) {
                cout << realId << " ";
                break;
            }
        }
    }
    cout << endl;
    
    return true;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);  // Using nullptr instead of 0 for modern C++

    string filename;
    
    cout << "Starting densest subgraph algorithm..." << endl;
    
    if (argc < 2) {
        cout << "Enter the filename (.edgelist): " << flush;
        getline(cin, filename);  // Using getline instead of cin >> to handle spaces
        
        // Check if input operation failed
        if (cin.fail()) {
            cerr << "Error: Failed to read input." << endl;
            return 1;
        }
    } else {
        filename = argv[1];
        cout << "Using command line argument filename: " << filename << endl;
    }
    
    // Trim whitespace and check if filename is empty
    if (!filename.empty()) {
        size_t start = filename.find_first_not_of(" \t\n\r");
        if (start != string::npos) {
            size_t end = filename.find_last_not_of(" \t\n\r");
            filename = filename.substr(start, end - start + 1);
        } else {
            filename = "";
        }
    }
    
    if (filename.empty()) {
        cerr << "Error: Filename cannot be empty." << endl;
        return 1;
    }
    
    cout << "Processing file: " << filename << endl;
    
    // Try to solve the problem
    if (!solve(filename)) {
        cerr << "Error: Failed to solve the problem." << endl;
        return 1;
    }
    
    cout << "Algorithm completed successfully." << endl;
    return 0;
}
