#include <string>
#include <unordered_map>

#include "myDijkstralib/graph.h"


namespace my_dijkstra_lib
{

void Graph::addEdge(const std::string& from, const std::string& to, unsigned int weight) noexcept{
    adj_list.try_emplace(from);
    adj_list.try_emplace(to);
    adj_list[from].push_back(Edge{to, weight}); // Храним каждое ребро в двух экземплярах
    adj_list[to].push_back(Edge{from, weight}); // для удобства при написании алгоритма
}

} // namespace my_dijkstra_lib
