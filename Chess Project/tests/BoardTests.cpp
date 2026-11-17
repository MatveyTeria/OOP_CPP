#include <gtest/gtest.h>
#include "chesslib/Board.h"

using chesslib::Color;
using chesslib::PieceType;

TEST(BoardFromFen, StartingPosition) {
    chesslib::Board a = chesslib::Board::start_position();
    EXPECT_EQ(a.piece_at(0), chesslib::make_piece(Color::White, PieceType::Rook));
    EXPECT_EQ(a.piece_at(1), chesslib::make_piece(Color::White, PieceType::Knight));
    EXPECT_EQ(a.piece_at(2), chesslib::make_piece(Color::White, PieceType::Bishop));
    EXPECT_EQ(a.piece_at(3), chesslib::make_piece(Color::White, PieceType::Queen));
    EXPECT_EQ(a.piece_at(4), chesslib::make_piece(Color::White, PieceType::King));
    for (int i = 8; i < 16; i++)
        EXPECT_EQ(a.piece_at(i), chesslib::make_piece(Color::White, PieceType::Pawn));
    for (int i = 16; i < 48; i++)
        EXPECT_EQ(a.piece_at(i), chesslib::kEmpty);
    for (int i = 48; i < 56; i++)
        EXPECT_EQ(a.piece_at(i), chesslib::make_piece(Color::Black, PieceType::Pawn));
    EXPECT_EQ(a.piece_at(63), chesslib::make_piece(Color::Black, PieceType::Rook));
}

TEST(BoardFromFen, AsymetricPosition) {
    chesslib::Board b = chesslib::Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    EXPECT_EQ(b.piece_at(chesslib::make_square(4,1)), chesslib::make_piece(Color::White, PieceType::Pawn));
    EXPECT_EQ(b.piece_at(chesslib::make_square(4,7)), chesslib::make_piece(Color::Black, PieceType::King));
    EXPECT_EQ(b.piece_at(chesslib::make_square(0,0)), chesslib::kEmpty);
    EXPECT_EQ(b.side_to_move(), Color::White);
}

TEST(BoardFromFen, BlackToMove) {
    chesslib::Board b = chesslib::Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 b - - 0 1");
    EXPECT_EQ(b.piece_at(chesslib::make_square(4,7)), chesslib::make_piece(Color::Black, PieceType::King));
    EXPECT_EQ(b.piece_at(chesslib::make_square(4,1)), chesslib::make_piece(Color::White, PieceType::Pawn));
    EXPECT_EQ(b.piece_at(chesslib::make_square(0,0)), chesslib::kEmpty);
    EXPECT_EQ(b.side_to_move(), Color::Black);
}

TEST(BoardFromFen, MoveCounter) {
    chesslib::Board b = chesslib::Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 b - - 40 4");
    EXPECT_EQ(b.halfmove_clock(), 40);
}

TEST(BoardFromFen, EmptyBoard) {
    chesslib::Board b = chesslib::Board::from_fen("8/8/8/8/8/8/8/8 w - - 40 4");
    for (int i = 0; i < 64; i++){
        EXPECT_EQ(b.piece_at(i), chesslib::kEmpty);
    }
}