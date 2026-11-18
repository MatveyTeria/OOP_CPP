#include "ConsoleBoardRenderer.hpp"

#include <iomanip>
#include <ostream>
#include <vector>

void ConsoleBoardRenderer::render(const Board& board,
                                  const std::vector<Robot>& robots,
                                  std::ostream& os) const {
    // Таблица «кто где» — чтобы не звать hasRobot(...) в двойном цикле.
    std::vector<std::vector<int>> robot_at(
        board.rows(), std::vector<int>(board.cols(), -1));
    for (const auto& r : robots) {
        const Position p = r.position();
        robot_at[p.row][p.col] = r.index();
    }

    os << "    ";
    for (int c = 0; c < board.cols(); ++c) {
        os << (c % 10);
    }
    os << '\n';

    for (int r = 0; r < board.rows(); ++r) {
        os << std::setw(3) << r << ' ';
        for (int c = 0; c < board.cols(); ++c) {
            const int idx = robot_at[r][c];
            if (idx >= 0) {
                os << static_cast<char>('0' + (idx % 10));
            } else if (board.hasCoin({r, c})) {
                os << '*';
            } else {
                os << '.';
            }
        }
        os << '\n';
    }
    os << '\n';
}