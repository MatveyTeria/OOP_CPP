#include <fstream>
#include <iostream>
#include <string>

#include "myDijkstralib/graphParser.h"
#include "myDijkstralib/DijkstraSolver.h"

using namespace my_dijkstra_lib;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <input_file|-> <start_vertex> <end_vertex>\n";
        return 1;
    }

    const std::string input_file  = argv[1];
    const std::string start_vertex = argv[2];
    const std::string end_vertex   = argv[3];

    GraphParser parser;
    Graph graph;

    if (input_file == "-") {
        graph = parser.parse(std::cin);
    } else {
        std::ifstream file(input_file);
        if (!file) {
            std::cerr << "Cannot open file: " << input_file << "\n";
            return 1;
        }
        graph = parser.parse(file);
    }

    DijkstraSolver solver(graph);
    auto path = solver.findShortestPath(start_vertex, end_vertex);

    if (path) {
        const auto& vertices = path->vertices;
        std::cout << "Vertices - {";
        for (std::size_t i = 0; i < vertices.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << vertices[i];
        }
        std::cout << "} - " << path->totalWeight << "\n";
    } else {
        std::cout << "No path found from " << start_vertex
                  << " to " << end_vertex << "\n";
    }

    return 0;
}