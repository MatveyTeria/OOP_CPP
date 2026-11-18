#include "GameResult.hpp"

#include <algorithm>
#include <limits>
#include <numeric>

namespace {

// -1 означает «робот не взял ни одной монеты». Для сортировки
// трактуем это как «бесконечно поздно» — такой игрок проигрывает
// по тай-брейку любому, кто взял хоть одну.
int lastTurnOrInf(int t) noexcept {
    return t < 0 ? std::numeric_limits<int>::max() : t;
}

} // namespace

std::vector<int> GameResult::ranking() const {
    // Возвращаем индексы players[] в порядке «лучший первым».
    // Не сортируем сами players — копия дороже и мы теряем
    // соответствие с индексами роботов.
    std::vector<int> order(players.size());
    std::iota(order.begin(), order.end(), 0);

    std::sort(order.begin(), order.end(), [this](int a, int b) {
        const RobotResult& pa = players[a];
        const RobotResult& pb = players[b];

        // 1. Очки — главный критерий.
        if (pa.score != pb.score) return pa.score > pb.score;

        // 2. Меньший ход последнего взятия — лучше.
        const int ta = lastTurnOrInf(pa.last_coin_turn);
        const int tb = lastTurnOrInf(pb.last_coin_turn);
        if (ta != tb) return ta < tb;

        // 3. Меньшее суммарное расстояние — лучше.
        if (pa.distance != pb.distance) return pa.distance < pb.distance;

        // 4. По имени — для детерминизма. Без этого std::sort мог бы
        //    переставлять равных игроков по-разному в разных запусках.
        return pa.name < pb.name;
    });
    return order;
}

std::vector<int> GameResult::winners() const {
    std::vector<int> out;
    if (players.empty()) return out;

    // Победитель — тот, у кого МАКСИМАЛЬНЫЙ score. Тай-брейки здесь
    // не применяются: победителей может быть несколько, и это честно.
    int best = players.front().score;
    for (const auto& p : players) {
        best = std::max(best, p.score);
    }
    for (int i = 0; i < static_cast<int>(players.size()); ++i) {
        if (players[i].score == best) out.push_back(i);
    }
    return out;
}