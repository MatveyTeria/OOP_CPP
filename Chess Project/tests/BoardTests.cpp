// tests/BoardMoveTest.cpp
#include <gtest/gtest.h>
#include "chesslib/Board.h"

using namespace chesslib;

namespace {
void expect_zobrist_consistent(const Board& b) {
    EXPECT_EQ(b.zobrist(), b.recompute_zobrist())
        << "Zobrist broken for FEN: " << b.to_fen();
}
} // namespace

TEST(BoardFromFen, StartingPosition) {
    Board a = Board::start_position();
    EXPECT_EQ(a.piece_at(0), make_piece(Color::White, PieceType::Rook));
    EXPECT_EQ(a.piece_at(1), make_piece(Color::White, PieceType::Knight));
    EXPECT_EQ(a.piece_at(2), make_piece(Color::White, PieceType::Bishop));
    EXPECT_EQ(a.piece_at(3), make_piece(Color::White, PieceType::Queen));
    EXPECT_EQ(a.piece_at(4), make_piece(Color::White, PieceType::King));
    for (int i = 8; i < 16; i++)
        EXPECT_EQ(a.piece_at(i), make_piece(Color::White, PieceType::Pawn));
    for (int i = 16; i < 48; i++)
        EXPECT_EQ(a.piece_at(i), kEmpty);
    for (int i = 48; i < 56; i++)
        EXPECT_EQ(a.piece_at(i), make_piece(Color::Black, PieceType::Pawn));
    EXPECT_EQ(a.piece_at(63), make_piece(Color::Black, PieceType::Rook));
}

TEST(BoardFromFen, AsymetricPosition) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    EXPECT_EQ(b.piece_at(make_square(4,1)), make_piece(Color::White, PieceType::Pawn));
    EXPECT_EQ(b.piece_at(make_square(4,7)), make_piece(Color::Black, PieceType::King));
    EXPECT_EQ(b.piece_at(make_square(0,0)), kEmpty);
    EXPECT_EQ(b.side_to_move(), Color::White);
}

TEST(BoardFromFen, BlackToMove) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 b - - 0 1");
    EXPECT_EQ(b.piece_at(make_square(4,7)), make_piece(Color::Black, PieceType::King));
    EXPECT_EQ(b.piece_at(make_square(4,1)), make_piece(Color::White, PieceType::Pawn));
    EXPECT_EQ(b.piece_at(make_square(0,0)), kEmpty);
    EXPECT_EQ(b.side_to_move(), Color::Black);
}

TEST(BoardFromFen, MoveCounter) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 b - - 40 4");
    EXPECT_EQ(b.halfmove_clock(), 40);
}

TEST(BoardFromFen, EmptyBoard) {
    Board b = Board::from_fen("8/8/8/8/8/8/8/8 w - - 40 4");
    for (int i = 0; i < 64; i++){
        EXPECT_EQ(b.piece_at(i), kEmpty);
    }
}

TEST(MakeMove, QuietKnight) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/8/4K1N1 w - - 0 1");
    Board a = b.make_move({make_square(6,0), make_square(5,2), {}, MoveType::Normal});
    EXPECT_EQ(a.piece_at(make_square(6,0)), kEmpty);
    EXPECT_EQ(a.piece_at(make_square(5,2)), make_piece(Color::White, PieceType::Knight));
    EXPECT_EQ(a.side_to_move(), Color::Black);
    EXPECT_EQ(a.ep_file(), -1);
    EXPECT_EQ(a.halfmove_clock(), 1);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, QuietPawnResetsHalfmove) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 7 1");
    Board a = b.make_move({make_square(4,1), make_square(4,2)});
    EXPECT_EQ(a.piece_at(make_square(4,1)), kEmpty);
    EXPECT_EQ(a.piece_at(make_square(4,2)), make_piece(Color::White, PieceType::Pawn));
    EXPECT_EQ(a.halfmove_clock(), 0);
    EXPECT_EQ(a.ep_file(), -1);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, DoublePawnPushSetsEp) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/4P3/4K3 w - - 0 1");
    Board a = b.make_move({make_square(4,1), make_square(4,3)});
    EXPECT_EQ(a.ep_file(), 4);                  // e-file
    EXPECT_EQ(a.piece_at(make_square(4,3)), make_piece(Color::White, PieceType::Pawn));
    expect_zobrist_consistent(a);
}

TEST(MakeMove, EpClearedByNonPawnMove) {
    // У чёрных «висит» ep-поле e3, но они играют конём
    Board b = Board::from_fen("4k3/8/8/8/8/8/8/4K1N1 b - e3 0 1");
    Board a = b.make_move({make_square(6,0), make_square(5,2)});
    EXPECT_EQ(a.ep_file(), -1);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, PawnCapture) {
    Board b = Board::from_fen("4k3/8/8/8/3p4/4P3/8/4K3 w - - 12 1");
    Board a = b.make_move({make_square(4,2), make_square(3,3)});
    EXPECT_EQ(a.piece_at(make_square(4,2)), kEmpty);
    EXPECT_EQ(a.piece_at(make_square(3,3)), make_piece(Color::White, PieceType::Pawn));
    EXPECT_EQ(a.halfmove_clock(), 0);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, PromotionAllPieces) {
    Board b = Board::from_fen("4k3/4P3/8/8/8/8/8/4K3 w - - 0 1");
    for (PieceType pt : {PieceType::Queen, PieceType::Rook,
                         PieceType::Bishop, PieceType::Knight}) {
        Board a = b.make_move({make_square(4,6), make_square(4,7), pt, MoveType::Promotion});
        EXPECT_EQ(a.piece_at(make_square(4,7)), make_piece(Color::White, pt));
        EXPECT_EQ(a.piece_at(make_square(4,6)), kEmpty);
        expect_zobrist_consistent(a);
    }
}

TEST(MakeMove, CapturePromotion) {
    Board b = Board::from_fen("3rK3/4P3/8/8/8/8/8/8 w - - 0 1");
    Board a = b.make_move({make_square(4,6), make_square(3,7),
                           PieceType::Queen, MoveType::Promotion});
    EXPECT_EQ(a.piece_at(make_square(3,7)), make_piece(Color::White, PieceType::Queen));
    EXPECT_EQ(a.halfmove_clock(), 0);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, EnPassantWhite) {
    // Белая пешка e5, чёрная только что сходила d7-d5, ep-цель d6
    Board b = Board::from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");
    Board a = b.make_move({make_square(4,4), make_square(3,5), {}, MoveType::EnPassant});
    EXPECT_EQ(a.piece_at(make_square(4,4)), kEmpty);                                    // e5
    EXPECT_EQ(a.piece_at(make_square(3,5)), make_piece(Color::White, PieceType::Pawn)); // d6
    EXPECT_EQ(a.piece_at(make_square(3,4)), kEmpty);                                    // d5 съедена
    EXPECT_EQ(a.halfmove_clock(), 0);
    EXPECT_EQ(a.ep_file(), -1);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, EnPassantBlack) {
    Board b = Board::from_fen("4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 1");
    Board a = b.make_move({make_square(3,3), make_square(4,2), {}, MoveType::EnPassant});
    EXPECT_EQ(a.piece_at(make_square(3,3)), kEmpty);                                    // d4
    EXPECT_EQ(a.piece_at(make_square(4,2)), make_piece(Color::Black, PieceType::Pawn)); // e3
    EXPECT_EQ(a.piece_at(make_square(4,3)), kEmpty);                                    // e4 съедена
    EXPECT_EQ(a.halfmove_clock(), 0);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, CastleWhiteKingside) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/8/4K2R w K - 0 1");
    Board a = b.make_move({make_square(4,0), make_square(6,0), {}, MoveType::Castle});
    EXPECT_EQ(a.piece_at(make_square(6,0)), make_piece(Color::White, PieceType::King));
    EXPECT_EQ(a.piece_at(make_square(5,0)), make_piece(Color::White, PieceType::Rook));
    EXPECT_EQ(a.piece_at(make_square(7,0)), kEmpty);
    EXPECT_EQ(a.castling() & Castle::WhiteKing, 0);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, CastleWhiteQueenside) {
    Board b = Board::from_fen("4k3/8/8/8/8/8/8/R3K3 w Q - 0 1");
    Board a = b.make_move({make_square(4,0), make_square(2,0), {}, MoveType::Castle});
    EXPECT_EQ(a.piece_at(make_square(2,0)), make_piece(Color::White, PieceType::King));
    EXPECT_EQ(a.piece_at(make_square(3,0)), make_piece(Color::White, PieceType::Rook));
    EXPECT_EQ(a.piece_at(make_square(0,0)), kEmpty);
    EXPECT_EQ(a.castling() & Castle::WhiteQueen, 0);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, CastleBlackKingside) {
    Board b = Board::from_fen("4k2r/8/8/8/8/8/8/4K3 b k - 0 1");
    Board a = b.make_move({make_square(4,7), make_square(6,7), {}, MoveType::Castle});
    EXPECT_EQ(a.piece_at(make_square(6,7)), make_piece(Color::Black, PieceType::King));
    EXPECT_EQ(a.piece_at(make_square(5,7)), make_piece(Color::Black, PieceType::Rook));
    EXPECT_EQ(a.castling() & Castle::BlackKing, 0);
    expect_zobrist_consistent(a);
}

TEST(MakeMove, CastleBlackQueenside) {
    Board b = Board::from_fen("r3k3/8/8/8/8/8/8/4K3 b q - 0 1");
    Board a = b.make_move({make_square(4,7), make_square(2,7), {}, MoveType::Castle});
    EXPECT_EQ(a.piece_at(make_square(2,7)), make_piece(Color::Black, PieceType::King));
    EXPECT_EQ(a.piece_at(make_square(3,7)), make_piece(Color::Black, PieceType::Rook));
    EXPECT_EQ(a.castling() & Castle::BlackQueen, 0);
    expect_zobrist_consistent(a);
}

TEST(CastlingRights, KingMoveClearsBothWhite) {
    Board b = Board::from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    Board a = b.make_move({make_square(4,0), make_square(4,1)}); // Ke2
    EXPECT_EQ(a.castling() & (Castle::WhiteKing | Castle::WhiteQueen), 0);
    EXPECT_EQ(a.castling() & (Castle::BlackKing | Castle::BlackQueen),
              Castle::BlackKing | Castle::BlackQueen);
    expect_zobrist_consistent(a);
}

TEST(CastlingRights, RookMoveClearsOnlyThatSide) {
    // Rh1-g1: снимает только WhiteKing
    Board b = Board::from_fen("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1");
    Board a = b.make_move({make_square(7,0), make_square(6,0)});
    EXPECT_EQ(a.castling() & Castle::WhiteKing, 0);
    EXPECT_NE(a.castling() & Castle::WhiteQueen, 0);
    expect_zobrist_consistent(a);
}

TEST(CastlingRights, RookCapturedClearsRight) {
    // Чёрная ладья берёт белую на a1 -> снимаются WhiteQueen и BlackQueen
    Board b = Board::from_fen("r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1");
    Board a = b.make_move({make_square(0,7), make_square(0,0)}); // Ra1
    EXPECT_EQ(a.castling() & Castle::WhiteQueen, 0);
    EXPECT_EQ(a.castling() & Castle::BlackQueen, 0);
    EXPECT_NE(a.castling() & Castle::BlackKing, 0);
    EXPECT_NE(a.castling() & Castle::WhiteKing, 0);
    expect_zobrist_consistent(a);
}

TEST(ToFen, StartingPosition) {
    Board b = Board::start_position();
    EXPECT_EQ(b.to_fen(),
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

TEST(ToFen, RoundTripMiddleGame) {
    // fullmove number не хранится, поэтому ставим "1"
    const std::string fen = "r1bqk2r/pppp1ppp/2n2n2/2b1p3/2B1P3/3P1N2/PPP2PPP/RNBQK2R w KQkq - 4 1";
    Board b = Board::from_fen(fen);
    EXPECT_EQ(b.to_fen(), fen);
    expect_zobrist_consistent(b);
}

TEST(ToFen, EpRoundTripBothSides) {
    // White to move: e.p.-target on rank 6
    EXPECT_EQ(Board::from_fen("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1").to_fen(),
              "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1");

    // Black to move: e.p.-target on rank 3
    EXPECT_EQ(Board::from_fen("4k3/8/8/8/3Pp3/8/8/4K3 b - d3 0 1").to_fen(),
              "4k3/8/8/8/3Pp3/8/8/4K3 b - d3 0 1");
}