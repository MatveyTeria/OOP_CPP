#include "chesslib/Board.h"

namespace
{
    uint8_t piece_from_char(char c) {
        switch (c)
        {
        case 'k':
            return make_piece(Color::White, PieceType::King);
        case 'q':
            return make_piece(Color::White, PieceType::Queen);
        case 'r':
            return make_piece(Color::White, PieceType::Rook);
        case 'b':
            return make_piece(Color::White, PieceType::Bishop);
        case 'n':
            return make_piece(Color::White, PieceType::Knight);
        case 'p':
            return make_piece(Color::White, PieceType::Pawn);
        
        case 'K':
            return make_piece(Color::Black, PieceType::King);
        case 'Q':
            return make_piece(Color::Black, PieceType::Queen);
        case 'R':
            return make_piece(Color::Black, PieceType::Rook);
        case 'B':
            return make_piece(Color::Black, PieceType::Bishop);
        case 'N':
            return make_piece(Color::Black, PieceType::Knight);
        case 'P':
            return make_piece(Color::Black, PieceType::Pawn);
        
        default:
            return kEmpty;
        } 
    }
} // namespace

namespace chesslib
{

static Board Board::start_position() {
    return from_fen(std::string_view("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
}

static Board Board::from_fen(std::string_view fen) {
    Board b;

    std::array<std::string_view, 6> fields;
    int field_num = 0;
    int last = 0;
    int i = 0;
    for (char c: fen) {
        if (c == ' ') {
            fields.at(field_num++) = std::string_view(fen.data() + last, i - last );
            last = i;
        }
        i++;
    }
    
    std::string_view placement = fields[0];

    int rank = 7;
    int file = 0;
    for (char c: placement) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if ('0' <= c && c <= '9') {
            file += c - '0';
        } else {
            b.cells_[rank * 8 + file] = piece_from_char(c);
        }
    }

    if (fields[1][0] == 'b')
        b.side_ = Color::Black;
    
    for (char c: fields[2]) {
        switch (c)
        {
        case 'q':
            b.castling_ |= 1;
            break;
        case 'k':
            b.castling_ |= 1 << 1;
            break;
        case 'W':
            b.castling_ |= 1 << 2;
            break;
        case 'K':
            b.castling_ |= 1 << 3;
            break;
        
        default:
            break;
        }
    }

    if (fields[3][0] != '-') 
        b.ep_file_ = fields[3][0] - '0';

    b.zobrist_ = b.recompute_zobrist();

    return b;
}

std::string Board::to_fen() const {}

Board Board::make_move(Move m) const {}

} // namespace chesslib