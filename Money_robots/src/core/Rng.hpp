#pragma once
#include <cstdint>
#include <random>

// Обёртка над std::mt19937. Решает одну задачу: превратить
// «seed как его задал пользователь» в «готовый движок».
//
// НЕ потокобезопасен: у каждого потока должен быть свой Rng.
// (std::mt19937 — как std::rand, но с явным состоянием.)
class Rng {
public:
    // seed == 0 → взять энтропию из std::random_device.
    // Иначе — детерминированный запуск.
    explicit Rng(std::uint32_t seed = 0)
        : engine_(resolveSeed(seed)) {}

    // Следующее 32-битное значение. Для выбора из N вариантов
    // % N даёт лёгкое смещение, но для игр это несущественно.
    [[nodiscard]] std::uint32_t next() noexcept {
        return static_cast<std::uint32_t>(engine_());
    }

    // Доступ к движку — для std::shuffle, std::uniform_int_distribution
    // и всего, что принимает UniformRandomBitGenerator.
    [[nodiscard]] std::mt19937& engine() noexcept { return engine_; }
    [[nodiscard]] const std::mt19937& engine() const noexcept { return engine_; }

private:
    static std::uint32_t resolveSeed(std::uint32_t requested) {
        if (requested != 0) return requested;
        std::random_device rd;
        return static_cast<std::uint32_t>(rd());
    }

    std::mt19937 engine_;
};