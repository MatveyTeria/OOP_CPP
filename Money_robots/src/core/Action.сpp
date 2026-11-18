#include "Action.hpp"

std::pair<int, int> deltaOf(Action a) noexcept {
    switch (a) {
        case Action::Up:    return {-1, 0};
        case Action::Down:  return {1, 0};
        case Action::Left:  return {0, -1};
        case Action::Right: return {0, 1};
        case Action::Stay:  return {0, 0};
    }
    return {0, 0};
}