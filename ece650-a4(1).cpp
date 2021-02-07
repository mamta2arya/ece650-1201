#include <minisat/core/Solver.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <algorithm>
#include <iterator>
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
    std::vector<std::pair<int, int>> edges;

    // an array containing adjacency lists
    std::list<int> *adj;

public:
    Graph(int V)
    {
        this->V = V;
        adj = new std::list<int>[V];
    }

    // for adding edge
    void add_edge(int v, int w)
    {
        adj[v].push_back(w);
        adj[w].push_back(v);

        edges.push_back(std::pair<int, int>{v, w});
    }

    void add_edge(std::pair<int, int> edge)
    {
        adj[edge.first].push_back(edge.second);
        adj[edge.second].push_back(edge.first);

        edges.push_back(edge);
    }

    int get_vertices_count()
    {
        return V;
    }

    std::vector<std::pair<int, int>> get_edges()
    {
        return edges;
    }
};

std::vector<int> findVertexCover(Graph graph, int cover_size)
{
    int vertices_count = graph.get_vertices_count();
    Minisat::Solver solver;
    std::vector<std::vector<Minisat::Lit>> matrix(vertices_count);

    for (int n = 0; n < vertices_count; ++n)
    {
        for (int k = 0; k < cover_size; ++k)
        {
            Minisat::Lit literal = Minisat::mkLit(solver.newVar());
            matrix[n].push_back(literal);
        }
    }

    // Rule 1: at least (exactly only one) one vertex is the ith vertex in the vertex cover, i in [1,k]
    for (int k = 0; k < cover_size; ++k)
    {
        Minisat::vec<Minisat::Lit> literals;
        for (int n = 0; n < vertices_count; ++n)
        { // n is changing first
            literals.push(matrix[n][k]);
        }
        solver.addClause(literals);
        literals.clear();
    }

    // Rule 2: no one vertex can appear twice in a vertex cover
    for (int n = 0; n < vertices_count; ++n)
    {
        for (int p = 0; p < cover_size - 1; ++p)
        {
            for (int q = p + 1; q < cover_size; ++q)
            {
                solver.addClause(~matrix[n][p], ~matrix[n][q]);
            }
        }
    }

    // Rule 3: no more than one vertex appears in the mth position of the vertex cover
    for (int m = 0; m < cover_size; ++m)
    {
        for (int p = 0; p < vertices_count - 1; ++p)
        {
            for (int q = p + 1; q < vertices_count; ++q)
            {
                solver.addClause(~matrix[p][m], ~matrix[q][m]);
            }
        }
    }

    std::vector<std::pair<int, int>> edges = graph.get_edges();

    //Rule 4: every edge is incident to at least one vertex in the vertex cover
    for (auto it = edges.begin(); it != edges.end(); ++it)
    {
        Minisat::vec<Minisat::Lit> literals;
        for (int k = 0; k < cover_size; ++k)
        {
            literals.push(matrix[(*it).first][k]);
            literals.push(matrix[(*it).second][k]);
        }
        solver.addClause(literals);
        literals.clear();
    }

    auto sat = solver.solve();

    if (sat)
    {
        std::vector<int> result;

        for (int i = 0; i < vertices_count; i++)
        {
            for (int j = 0; j < cover_size; ++j)
            {
                if (Minisat::toInt(solver.modelValue(matrix[i][j])) == 0)
                {
                    result.push_back(i);
                }
            }
        }
        return result;
    }
    else
    {
        return {-1};
    }
}

void printMinLengthVertexCover(Graph graph)
{
    std::vector<int> result;
    std::vector<int> tmp_result;
    std::vector<int> tmp{-1};

    int start = 1;
    int end = graph.get_vertices_count();

    while (start <= end)
    {

        int mid = floor((start + end) / 2);

        tmp_result = findVertexCover(graph, mid);

        bool coverExists = std::equal(tmp_result.begin(), tmp_result.end(), tmp.begin());

        if (not coverExists)
        {
            end = mid - 1;
            result.clear();
            result = tmp_result;
        }
        else
        {
            start = mid + 1;
        }
    }

    sort(result.begin(), result.end());

    for (unsigned int i = 0; i < result.size(); ++i)
    {
        std::cout << result[i] << " ";
    }
    std::cout << "\n";
}

int main()
{
    std::string line;
    char cmd;
    int vertices_count;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::string> line_split;
    Graph g(0);
    bool graph_created = true;

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
            for (unsigned int i = 0; i < edges.size(); i++)
            {
                int v = edges[i].first;
                int w = edges[i].second;
                if (v >= vertices_count || w >= vertices_count)
                {
                    graph_created = false;
                    std::cerr << "Error : edge (" << v << "," << w << ") is invalid" << std::endl;
                    break;
                }
                g.add_edge(v, w);
            }

            if (graph_created)
                printMinLengthVertexCover(g);

            break;
        default:
            std::cerr << "Error: command not recognized" << std::endl;
            break;
        }
    }

    return 0;
}
