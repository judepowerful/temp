#include <iostream>
#include <string>
#include <list>
#include <map>
#include <regex>
#include <queue>
#include <algorithm>

using namespace std;

// create Graph class to map the corresponding case
class Graph {
public:
    Graph() = default;

    // Function to add a vertex
    void addVertex(int v) {
        vertices.push_back(v);
    }

    // Function to add an edge
    void addEdge(int u, int v) {
        if (std::find(vertices.begin(), vertices.end(), u) != vertices.end()
            && std::find(vertices.begin(), vertices.end(), v) != vertices.end()) {
            adjacencyList[u].push_back(v);
            adjacencyList[v].push_back(u);
        } else {
            // report Error if there is at least a vertex not found
            cerr << "Error: Edge <" << u << "," << v << "> references vertices that do not exist." << endl;
            return;
        }
    }

    // Function to reset the graph
    void clearGraph(){
        adjacencyList.clear();
        vertices.clear();
    }

    // Find the shortest path using breadth first search
    vector<int> findShortestPath(int start, int end) {
        if (std::find(vertices.begin(), vertices.end(), start) == vertices.end()
            || std::find(vertices.begin(), vertices.end(), end) == vertices.end()) {
            cerr << "Error: You are searching a path where at least one end does not exist." << endl;
            return {};
        }

        queue<int> q;
        map<int, bool> visited;
        map<int, int> parent;

        q.push(start);
        visited[start] = true;
        parent[start] = -1;

        while (!q.empty()) {
            int current = q.front();
            q.pop();

            if (current == end) {
                vector<int> path;
                while (current != -1) {
                    path.insert(path.begin(), current);
                    current = parent[current];
                }
                return path;
            }

            for (int neighbor : adjacencyList[current]) {
                if (!visited[neighbor]) {
                    q.push(neighbor);
                    visited[neighbor] = true;
                    parent[neighbor] = current;
                }
            }
        }

        // report Error if cant find a path
        cerr << "Error: There is no path between " << start << " and " << end << "." << endl;
        return {};
    }

private:
    map<int, vector<int>> adjacencyList;
    vector<int> vertices;
};


int main()
{
    Graph g;
    while(!std::cin.eof())
    {
        string input;

        // read line
        getline(cin,input);
        // ss the input
        stringstream ss(input);


        // reading of the input split by empty space
        vector <string> reading;

        // split by empty space
        string split;
        while(getline(ss, split, ' '))
        {
            reading.push_back(split);
        }

        // command V
        if (reading[0] == "V")
        {
            // clear graph
            g.clearGraph();

            // number of vertices
            int num_v;
            stringstream(reading[1])>>num_v;
            for (int i = 1; i <= num_v; i++) {
                g.addVertex(i);
            }

        }
            // command E
        else if (reading[0]== "E")
        {
            vector<pair<int, int>> edges;

            // Use regular expression to match pairs in the input
            std::regex pattern(R"(<(\d+),(\d+)>)");
            std::smatch matches;

            // Search for pairs in the input
            auto input_begin = reading[1].cbegin();
            while (std::regex_search(input_begin, reading[1].cend(), matches, pattern)) {
                int first = std::stoi(matches[1]);
                int second = std::stoi(matches[2]);
                edges.emplace_back(first, second);
                input_begin = matches.suffix().first;
            }

            // add the edges
            for (const auto& edge : edges) {
                g.addEdge(edge.first, edge.second);
            }


        }
            // command s
        else if (reading[0] == "s")
        {
            int start, end;
            stringstream(reading[1]) >> start;
            stringstream(reading[2]) >> end;

            vector<int> shortestPath = g.findShortestPath(start, end);

            if (shortestPath.empty()) {
                // no path found because there is an error
            } else {
                // print the shortest path
                for (int i = 0; i < shortestPath.size(); i++) {
                    if (i > 0) {
                        cout << "-";
                    }
                    cout << shortestPath[i];
                }
                cout << endl;
            }
        }
    }
}