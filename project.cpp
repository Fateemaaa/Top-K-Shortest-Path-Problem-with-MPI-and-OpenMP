#include <omp.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <mpi.h>

#include<climits>
using namespace std;

// Utility function to split strings by a delimiter
vector<string> tokenize(const string& str, char delimiter) {
    vector<string> result;
    string segment;
    istringstream stream(str);
    while (getline(stream, segment, delimiter)) {
        result.push_back(segment);
    }
    return result;
}

class NetworkGraph {
public:
    unordered_map<string, unordered_map<string, int>> connections;

    void insertEdge(const string& fromNode, const string& toNode, int weight) {
        connections[fromNode][toNode] = weight;
        connections[toNode][fromNode] = weight; // Assuming the graph is undirected
    }

   
    vector<vector<int>> initializeDistanceMatrix() {
        // Mapping nodes to matrix indices
        unordered_map<string, int> nodeIndex;
        int index = 0;
        for (auto& node : connections) {
            if (nodeIndex.find(node.first) == nodeIndex.end()) {
                nodeIndex[node.first] = index++;
            }
        }

        int n = nodeIndex.size();
        vector<vector<int>> distanceMatrix(n, vector<int>(n, INT_MAX));

        // Setting diagonal to zero
        for (int i = 0; i < n; ++i) {
            distanceMatrix[i][i] = 0;
        }

        // Setting distances for direct connections
        for (auto& node : connections) {
            int fromIndex = nodeIndex[node.first];
            for (auto& adj : node.second) {
                int toIndex = nodeIndex[adj.first];
                distanceMatrix[fromIndex][toIndex] = adj.second;
            }
        }

        return distanceMatrix;
    }


   

    // Retrieves up to K shortest paths from a source to a target
    vector<vector<string>> retrieveKShortestPaths(const string& start, const string& end, int k) {
        struct Route {
            int totalCost;
            vector<string> nodes;

            bool operator>(const Route& other) const {
                return totalCost > other.totalCost;
            }
        };

        priority_queue<Route, vector<Route>, greater<Route>> priorityQueue;
        unordered_map<string, vector<Route>> shortestPaths;
        shortestPaths[start].push_back({0, {start}});
        priorityQueue.push({0, {start}});

        bool done = false;

        #pragma omp parallel
        {
            while (true) {
                Route currentRoute;
                #pragma omp critical
                {
                    if (priorityQueue.empty()) {
                        done = true;
                    } else {
                        currentRoute = priorityQueue.top();
                        priorityQueue.pop();
                    }
                }

                if (done) break;

                string currentEndNode = currentRoute.nodes.back();

                if (currentRoute.nodes.size() > 1 && currentEndNode == end && shortestPaths[end].size() >= k)
                    continue;

                for (const auto& adjacent : connections[currentEndNode]) {
                    Route newRoute = currentRoute;
                    newRoute.totalCost += adjacent.second;
                    newRoute.nodes.push_back(adjacent.first);

                    if (shortestPaths[adjacent.first].size() < k) {
                        #pragma omp critical
                        {
                            shortestPaths[adjacent.first].push_back(newRoute);
                            priorityQueue.push(newRoute);
                        }
                    }
                }
            }
        }

        vector<vector<string>> resultPaths;
        for (const auto& path : shortestPaths[end]) {
            resultPaths.push_back(path.nodes);
        }
        return resultPaths;
    }
};

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int processRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);
    int numProcesses;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    NetworkGraph graph;
    if (processRank == 0) {
        ifstream dataFile("doctorwho.csv");
        if (!dataFile.is_open()) {
            cerr << "Failed to open file" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        string dataLine;
        while (getline(dataFile, dataLine)) {
            vector<string> data = tokenize(dataLine, ',');
            if (data.size() == 4) {
                try {
                    int edgeWeight = stoi(data[2]);
                    graph.insertEdge(data[0], data[1], edgeWeight);
                } catch (const std::invalid_argument& err) {
                    cerr << "Invalid argument: " << err.what() << " in line: " << dataLine << endl;
                    continue;
                } catch (const std::out_of_range& err) {
                    cerr << "Value out of range: " << err.what() << " in line: " << dataLine << endl;
                    continue;
                }
            }
        }
        dataFile.close();
    }

    

    string sourceNode = "Rosa Parks";
    string destinationNode = "Gat";
    int pathCount = 10;

    vector<vector<string>> foundPaths;
   
       
   

    // Output or further process results
    if (processRank == 0) {
     vector<vector<int>> distanceMatrix = graph.initializeDistanceMatrix();
foundPaths = graph.retrieveKShortestPaths(sourceNode, destinationNode, pathCount);       
   
        for (size_t i = 0; i < foundPaths.size(); ++i) {
            cout << "Route " << i + 1 << ": ";
            for (const string& node : foundPaths[i]) {
                cout << node << " -> ";
            }
            cout << "end" << endl;
        }
    }
   

    MPI_Finalize();
    return 0;
}
