#include "Board.hpp"

#include <stdexcept>

Board::Board(int rows, int cols)
    : rows_(rows), cols_(cols)
{
    if (rows <= 0 || cols <= 0) {
        throw std::invalid_argument("Board: rows and cols must be positive");
    }
    const std::size_t total =
        static_cast<std::size_t>(rows_) * static_cast<std::size_t>(cols_);
    coins_.assign(total, std::uint8_t{0});
}

std::size_t Board::indexOf(Position p) const {
    // Единственное место в классе, где проверяются границы.
    // Всё остальное полагается на то, что indexOf уже бросил, если надо.
    if (p.row < 0 || p.row >= rows_ || p.col < 0 || p.col >= cols_) {
        throw std::out_of_range("Board: position out of bounds");
    }
    return static_cast<std::size_t>(p.row) * static_cast<std::size_t>(cols_)
           + static_cast<std::size_t>(p.col);
}

bool Board::hasCoin(Position p) const {
    return coins_[indexOf(p)] != 0;
}

void Board::placeCoin(Position p) {
    // Идемпотентна: повторный вызов на занятую клетку — no-op.
    // Это осознанно: инициализатор поля кладёт монеты «пачкой»,
    // и проверять каждую было бы шумом.
    coins_[indexOf(p)] = 1;
}

bool Board::takeCoin(Position p) {
    auto& cell = coins_[indexOf(p)];
    if (cell == 0) return false;
    cell = 0;
    return true;
}

int Board::remainingCoins() const noexcept {
    // O(rows*cols). Для полей до 100×100 — мгновенно.
    // Если бы это было узким местом, хранили бы счётчик полем
    // и инкрементировали/декрементировали в place/take.
    int count = 0;
    for (auto c : coins_) {
        if (c) ++count;
    }
    return count;
}

std::vector<Position> Board::coinPositions() const {
    std::vector<Position> out;
    out.reserve(static_cast<std::size_t>(remainingCoins()));

    // Порядок обхода — строго построчный (row-major). Это ВАЖНО:
    // стратегии, которые берут coins.front() и tie-break'ают по порядку,
    // получат детерминированное поведение. Не меняйте порядок без причины.
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (coins_[static_cast<std::size_t>(r) * static_cast<std::size_t>(cols_)
                      + static_cast<std::size_t>(c)]) {
                out.push_back({r, c});
            }
        }
    }
    return out;
}