#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <regex>
#include <bits/stdc++.h>

std::vector<std::pair<int, int>> parse_edges(std::string str)
{
    std::pair<int, int> edge;
    std::vector<std::pair<int, int>> edges;

    // using regex
    try
    {
        std::regex re("-?[0-9]+"); //match consectuive numbers
        std::sregex_iterator next(str.begin(), str.end(), re);
        std::sregex_iterator end;
        while (next != end)
        {
            std::smatch match1;
            std::smatch match2;

            match1 = *next;
            next++;
            // iterate to next match
            if (next != end)
            {
                match2 = *next;
                edge.first = std::stoi(match1.str());
                edge.second = std::stoi(match2.str());
                edges.push_back(edge);
                next++;
            }
        }
    }
    catch (std::regex_error &e)
    {
        edges.clear();
    }
    return edges;
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// This class represents a undirected graph using adjacency list 
class Graph
{
    int V; // Number of vertices

    // an array containing adjacency lists 
    
    std::list<int> *adj;

public:
    Graph(int V); 
    // for adding edge
    void add_edge(int v, int w);
   // BFS traversing from source to destination
    bool do_BFS(int src, int dest, int predecessor[], int s_distance[]);
   // shortest path between source and destination   
    void print_shortest_path(int src, int dest);
};

Graph::Graph(int V)
{
    this->V = V;
    adj = new std::list<int>[V];
}

void Graph::add_edge(int v1, int v2)
{
    adj[v1].push_back(v2); 
    adj[v2].push_back(v1);
}

bool Graph::do_BFS(int src, int dest, int predecessor[], int s_distance[])
{
    // a boolean array which stores the the vertex which is traverse at least once in the BFS algorithm
    
    bool *visited = new bool[V];

    // initially all vertices are unvisited 

    // and as no path is there
    // distance[i] for all i set to infinity
    for (int i = 0; i < V; i++)
    {
        visited[i] = false;
        s_distance[i] = INT_MAX;
        predecessor[i] = -1;
    }

    // Create a queue to maintain queue of vertices whose
    // adjacency list is to be scanned as per normal
    // BFS algorithm
    std::list<int> queue;

    // Mark the current node as visited and enqueue it
    visited[src] = true;
    s_distance[src] = 0;
    queue.push_back(src);

    // 'it' will be used to get all adjacent
    // vertices of a vertex
    std::list<int>::iterator it;

    while (!queue.empty())
    {
        // Dequeue a vertex from queue and print it
        int curr_v = queue.front();
        queue.pop_front();

        // Get all adjacent vertices of the dequeued
        // vertex s. If a adjacent has not been visited,
        // then mark it visited and enqueue it
        for (it = adj[curr_v].begin(); it != adj[curr_v].end(); ++it)
        {
            int adj_v = *it;
            if (!visited[adj_v])
            {
                visited[adj_v] = true;
                s_distance[adj_v] = s_distance[curr_v] + 1;
                predecessor[adj_v] = curr_v;
                queue.push_back(adj_v);

                // We stop BFS when we find
                // destination.
                if (adj_v == dest)
                    return true;
            }
        }
    }

    return false;
}

void Graph::print_shortest_path(int src, int dest)
{
    // predecessor[i] array stores predecessor of
    // i and distance array stores distance of i
    // from s
    int predecessor[V], s_distance[V];

    if (do_BFS(src, dest, predecessor, s_distance) == false)
    {
        std::cerr << "Error : Given source and destination are not connected" << std::endl;
        return;
    }

    // vector path stores the shortest path
    std::vector<int> shortest_path;
    int curr_v = dest;
    shortest_path.push_back(curr_v);
    while (predecessor[curr_v] != -1)
    {
        shortest_path.push_back(predecessor[curr_v]);
        curr_v = predecessor[curr_v];
    }

    // printing path from source to destination
    for (int i = shortest_path.size() - 1; i >= 0; i--)
    {
        if (i != 0)
        {

            std::cout << shortest_path[i] << "-";
        }
        else
        {

            std::cout << shortest_path[i] << std::endl;
        }
    }
}

int main()
{
    std::string line;
    char cmd;
    int vertices_count;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::string> line_split;
    int src, dest;
    Graph g(0);

    while (std::getline(std::cin, line))
    {
        cmd = line[0];
        switch (cmd)
        {
        case 'V':
        case 'v':
            /* code for adding vertices to new graph */
            // V 15
            vertices_count = stoi(line.substr(2, std::string::npos), nullptr, 10);
            g = Graph(vertices_count);
            break;

        case 'E':
        case 'e':
            /* code for adding edges to new graph*/
            // E {<0,2>,<2,1>,<2,3>,<3,4>,<4,1>}
            edges = parse_edges(line.substr(2, std::string::npos));
            for (int i = 0; i < edges.size(); i++)
            {
                int v = edges[i].first;
                int w = edges[i].second;
                if (v >= vertices_count || w >= vertices_count)
                {
                    std::cerr << "Error : edge (" << v << "," << w << ") is invalid" << std::endl;
                    break;
                }
                g.add_edge(v, w);
            }
            break;

        case 'S':
        case 's':
            /* code for getting shortest path */
            // s 2 10
            line_split = split(line.substr(2, std::string::npos), ' ');
            src = stoi(line_split.at(0), nullptr, 10);
            dest = stoi(line_split.at(1), nullptr, 10);

            if (src >= vertices_count || dest >= vertices_count)
            {
                std::cerr << "Error : either source or destination vertex is invalid" << std::endl;
                break;
            }

            g.print_shortest_path(src, dest);
            break;

        default:
            std::cerr << "Error: command not recognized" << std::endl;
            break;
        }
    }

    return 0;
}