#include <iostream>
#include <fstream>
#include <string>

#include "myDijkstralib\edge.h"
#include "myDijkstralib\graph.h"
#include "myDijkstralib\graphParser.h"
#include "myDijkstralib\path.h"
#include "myDijkstralib\DijkstraSolver.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <start_vertex> <end_vertex>" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    std::string start_vertex = argv[2];
    std::string end_vertex = argv[3];

    std::istringstream in;
    if (input_file == "-") {
        in.rdbuf(std::cin.rdbuf());
    } else {
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Cannot open file " << input_file << std::endl;
            return 1;
        }
        in.rdbuf(file.rdbuf());
    }

    auto parser = GraphParser{};
    auto graph = parser.parse(in);

    DijkstraSolver solver(graph);
    auto path = solver.findShortestPath(start_vertex, end_vertex);

    if (path.has_value()) {
        std::cout << "Вершины - {";
        for (const auto& vertex : path.value().verticies) {
            std::cout << vertex;
            if (vertex != path.value().verticies.back()) {
                std::cout << ", ";
            }
        }
        std::cout << "} - " << path.value().totalWeight << std::endl;
    } else {
        std::cout << "No path found from " << start_vertex << " to " << end_vertex << std::endl;
    }

    return 0;
}
