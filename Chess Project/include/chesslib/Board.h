#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <string_view>

namespace chesslib
{

using Square = uint8_t; // 0-63, a1=0 - h8=63

enum class Color : uint8_t { White = 0, Black = 1 };
enum class PieceType : uint8_t { Pawn = 0, Knight, Bishop, Rook, Queen, King };

constexpr uint8_t kEmpty = 12;

constexpr uint8_t make_piece(Color c, PieceType t) noexcept {
    return uint8_t(c) * 6 + uint8_t(t);
}

// ВНИМАНИЕ: определены только для реальных фигур (0..11), не для kEmpty.
// Вызов на kEmpty даст «(Color)2 / (PieceType)0», что чушь.
constexpr Color     color_of(uint8_t p) noexcept { return Color(p / 6); }
constexpr PieceType type_of (uint8_t p) noexcept { return PieceType(p % 6); }
constexpr bool is_empty(uint8_t p) noexcept { return p == kEmpty; }

constexpr int  file_of(Square s) noexcept { return s % 8; }
constexpr int  rank_of(Square s) noexcept { return s / 8; }
constexpr Square make_square(int file, int rank) noexcept {
    return Square(rank * 8 + file);
}

// Права рокировки как битовая маска. Порядок битов:
//   q k Q K  — совместим с алгоритмом в from_fen, но порядок в FEN-строке
//   стандартный KQkq (это только про сериализацию, к битам отношения не имеет).
enum Castle : uint8_t {
    BlackQueen = 1u << 0,   // 'q'
    BlackKing  = 1u << 1,   // 'k'
    WhiteQueen = 1u << 2,   // 'Q'
    WhiteKing  = 1u << 3,   // 'K'
};

enum class MoveType : uint8_t { Normal, Promotion, EnPassant, Castle };

struct Move {
    Square from;
    Square to;
    PieceType promo = PieceType::Queen;
    MoveType type  = MoveType::Normal;
    constexpr bool operator==(const Move&) const = default;
};

static constexpr std::array<uint8_t, 64> make_empty() {
    std::array<uint8_t, 64> a{};
    for (auto& c : a) c = chesslib::kEmpty;
    return a;
}

class Board {
public:
    static Board start_position();
    static Board from_fen(std::string_view fen);
    std::string to_fen() const noexcept;

    uint8_t  piece_at(Square sq) const noexcept { return cells_[sq]; }
    Color    side_to_move()    const noexcept { return side_; }
    uint8_t  castling()        const noexcept { return castling_; }
    int8_t   ep_file()         const noexcept { return ep_file_; }
    uint8_t  halfmove_clock()  const noexcept { return halfmove_clock_; }
    uint64_t zobrist()         const noexcept { return zobrist_; }

    uint64_t recompute_zobrist() const noexcept;

    Board make_move(Move m) const noexcept;

private:
    std::array<uint8_t, 64> cells_{};
    uint64_t zobrist_ = 0;
    uint8_t  castling_ = 0;
    int8_t   ep_file_  = -1;
    Color    side_     = Color::White;
    uint8_t  halfmove_clock_ = 0;
};

} // namespace chesslib