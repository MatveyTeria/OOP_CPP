#pragma once
#include <utility>

enum class Action { Up, Down, Left, Right, Stay };

// Возвращает изменение координат; Stay → (0,0)
[[nodiscard]] std::pair<int,int> deltaOf(Action a) noexcept;