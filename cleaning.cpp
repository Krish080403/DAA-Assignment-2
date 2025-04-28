#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <algorithm>

using namespace std;

// Function to clean and reindex a graph
pair<int, int> clean_and_reindex(const string& input_path, const string& output_path) {
    set<pair<int, int>> edges;
    set<int> nodes;

    ifstream infile(input_path);
    if (!infile.is_open()) {
        cerr << "Error opening file: " << input_path << endl;
        exit(1);
    }

    string line;
    while (getline(infile, line)) {
        line.erase(remove(line.begin(), line.end(), '\r'), line.end()); // Handle CR for Windows files
        line.erase(remove(line.begin(), line.end(), '\n'), line.end()); // Remove newline

        if (line.empty() || line[0] == '#')
            continue;

        istringstream iss(line);
        int u, v;
        if (!(iss >> u >> v))
            continue; // Not enough parts

        if (u == v)
            continue; // Skip self-loop

        if (u > v)
            swap(u, v); // Always store smaller node first to avoid duplicates

        edges.insert({u, v});
        nodes.insert(u);
        nodes.insert(v);
    }
    infile.close();

    // Map old node ids to 0...N-1
    unordered_map<int, int> mapping;
    int idx = 0;
    for (int node : nodes) {
        mapping[node] = idx++;
    }

    // Write cleaned edges
    ofstream outfile(output_path);
    if (!outfile.is_open()) {
        cerr << "Error writing to file: " << output_path << endl;
        exit(1);
    }

    for (const auto& edge : edges) {
        outfile << mapping[edge.first] << " " << mapping[edge.second] << endl;
    }
    outfile.close();

    return {static_cast<int>(nodes.size()), static_cast<int>(edges.size())};
}

int main() {
    vector<pair<string, string>> datasets = {
        //{"netscience.txt", "netscience_clean.edgelist"},
        //{"as19991210.txt", "as19991210_clean.edgelist"},
        //{"CA-HepTh.txt", "CA-HepTh_clean.edgelist"},
        //{"as-caida20071105.txt", "as-caida20071105_clean.edgelist"},
        {"yeast.txt","yeast_clean.edgelist"}
    };

    cout << "Cleaned Datasets Summary:" << endl;
    cout << "Input File\tOutput File\tNodes\tEdges" << endl;

    for (const auto& dataset : datasets) {
        auto [nodes, edges] = clean_and_reindex(dataset.first, dataset.second);
        cout << dataset.first << "\t" << dataset.second << "\t" << nodes << "\t" << edges << endl;
    }

    return 0;
}

