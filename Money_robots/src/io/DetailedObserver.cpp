#include "DetailedObserver.hpp"

#include <istream>
#include <ostream>
#include <string>

namespace {

const char* actionName(Action a) noexcept {
    switch (a) {
        case Action::Up:    return "Up";
        case Action::Down:  return "Down";
        case Action::Left:  return "Left";
        case Action::Right: return "Right";
        case Action::Stay:  return "Stay";
    }
    return "?";
}

} // namespace

DetailedObserver::DetailedObserver(std::istream& in, std::ostream& out)
    : in_(in), out_(out) {}

void DetailedObserver::onGameStart(const Board& board,
                                   const std::vector<Robot>& robots) {
    out_ << "=== Game started ===\n";
    renderer_.render(board, robots, out_);
}

void DetailedObserver::onTurnStart(int turn,
                                   const std::vector<Robot>& robots) {
    out_ << "-- Turn " << (turn + 1) << " --\n";
    for (const auto& r : robots) {
        out_ << "  " << r.name() << " @ ("
             << r.position().row << ',' << r.position().col
             << ") score=" << r.score() << '\n';
    }
}

void DetailedObserver::onTurnEnd(int,
                                 const std::vector<Action>& actions,
                                 const Board& board,
                                 const std::vector<Robot>& robots) {
    renderer_.render(board, robots, out_);

    out_ << "Actions: ";
    for (std::size_t i = 0; i < actions.size(); ++i) {
        if (i) out_ << ", ";
        out_ << robots[i].name() << '=' << actionName(actions[i]);
    }
    out_ << "\nPress Enter to continue...";
    out_.flush();   // важно: пользователь должен увидеть «Press Enter» до get()
    in_.get();      // один символ; EOF безопасен (вернёт EOF, продолжим)
}