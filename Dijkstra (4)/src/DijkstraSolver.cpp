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
    using QueueItem = std::pair<std::uint64_t, std::string>;
    std::priority_queue<
        QueueItem, std::vector<QueueItem>,
        std::greater<>> pr_queue;
    
    pr_queue.push(QueueItem{0, from});
    
}

} // namespace my_dijkstra_lib

