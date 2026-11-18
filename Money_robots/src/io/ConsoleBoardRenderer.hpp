#pragma once

#include <iosfwd>
#include <vector>

#include "Board.hpp"
#include "Robot.hpp"

class ConsoleBoardRenderer {
public:
    void render(
        const Board& board,
        const std::vector<Robot>& robots,
        std::ostream& os) const;
};
