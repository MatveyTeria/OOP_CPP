#include <queue>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <limits>
#include <optional>
#include <utility>
#include <functional>
#include <algorithm>

#include "myDijkstralib/DijkstraSolver.h"

namespace my_dijkstra_lib
{

std::optional<Path> DijkstraSolver::findShortestPath(const std::string& from, const std::string& to) const {
    if (!graph.hasVertex(from))
        return std::nullopt;

    using QueueItem = std::pair<std::uint64_t, std::string>;
    std::priority_queue<
        QueueItem, std::vector<QueueItem>,
        std::greater<>> pr_queue;

    std::unordered_map<std::string, std::uint64_t> real_weights;

    std::unordered_map<std::string, std::string> predecessors;
    
    pr_queue.push(QueueItem{0, from});
    real_weights[from] = 0;
    predecessors[from] = from;
    do {
        QueueItem closest = pr_queue.top();
        if (closest == top)
            break;
        pr_queue.pop();
        std::string vertex_name = closest.second;
        std::uint64_t vertex_weight = closest.first;

        if (real_weights.at(vertex_name) != vertex_weight) continue;

        const auto& adjacent = graph.getAdjacentEdges(vertex_name);
        for (const Edge& e: adjacent) {
            if (real_weights.find(e.to) == real_weights.end() || 
                vertex_weight + e.weight < real_weights[e.to] ) {
                real_weights[e.to] = vertex_weight + e.weight;
                predecessors[e.to] = vertex_name;
                pr_queue.push(QueueItem{vertex_weight + e.weight, e.to});
            }
        }
    } while (!pr_queue.empty());

    if (real_weights.find(to) == real_weights.end())
        return std::nullopt;
    
    std::vector<std::string> verticies;
    verticies.push_back(to);
    std::string cur = to;
    while (cur != from) {
        verticies.push_back(cur);
        cur = predecessors.at(cur);
    }
    verticies.push_back(cur);
    std::reverse(verticies.begin(), verticies.end());
    return Path{std::move(verticies), real_weights.at(to)};
}

} // namespace my_dijkstra_lib

