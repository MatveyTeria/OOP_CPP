#include <string>
#include <unordered_map>

#include "myDijkstralib/graph.h"


namespace my_dijkstra_lib
{

void Graph::addEdge(const std::string& from, const std::string& to, unsigned int weight) noexcept{
    if (from == to) { // Loop - needs to be added once
        adj_list.try_emplace(from);
        adj_list[from].push_back(Edge{to, weight});
        return;
    }
    adj_list.try_emplace(from);
    adj_list.try_emplace(to);
    adj_list[from].push_back(Edge{to, weight}); // Storing 2 copies of the edge
    adj_list[to].push_back(Edge{from, weight}); // assuming the graph is undirected
}

} // namespace my_dijkstra_lib
