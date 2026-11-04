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
constexpr bool is_empty(uint8_t p) noexcept { return p == kEmpty; }
constexpr int  file_of(Square s) noexcept { return s % 8; }
constexpr int  rank_of(Square s) noexcept { return s / 8; }
constexpr Square make_square(int file, int rank) noexcept {
    return Square(rank * 8 + file);
}

enum class MoveType : uint8_t { Normal, Promotion, EnPassant, Castle };

struct Move {
    Square from;
    Square to;
    PieceType promo = PieceType::Queen;
    MoveType type  = MoveType::Normal;

    constexpr bool operator==(const Move&) const = default; // For testing
};

class Board {
public:
    static Board start_position();

    static Board from_fen(std::string_view fen); // For testing
    std::string to_fen() const noexcept; // For testing

    uint8_t  piece_at(Square sq) const noexcept { return cells_[sq]; }
    Color    side_to_move()    const noexcept { return side_; }
    uint8_t  castling()        const noexcept { return castling_; }
    int8_t   ep_file()         const noexcept { return ep_file_; }
    uint8_t  halfmove_clock()  const noexcept { return halfmove_clock_; }
    uint64_t zobrist()         const noexcept { return zobrist_; }

    Board make_move(Move m) const; // Main method

private:
    std::array<uint8_t, 64> cells_{}; // 0-63, a1=0 - h8=63
    uint64_t zobrist_ = 0; // hashing for transposition table
    uint8_t  castling_ = 0; // 0bKQkq
    int8_t   ep_file_  = -1; // en passant file or -1 if no en passant is possible
    Color    side_     = Color::White; // side to move
    uint8_t  halfmove_clock_ = 0; // move counter for 50 move draw rule
};

} // namespace chesslib