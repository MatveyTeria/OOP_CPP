#include "chesslib/Board.h"

#include <stdexcept>

namespace
{
    uint8_t piece_from_char(char c) {
        chesslib::Color color = chesslib::Color::Black;
        if ('A' <= c && c <= 'Z') {
            color = chesslib::Color::White;
            c = c - 'A' + 'a';
        }
        switch (c)
        {
        case 'k':
            return make_piece(color, chesslib::PieceType::King);
        case 'q':
            return make_piece(color, chesslib::PieceType::Queen);
        case 'r':
            return make_piece(color, chesslib::PieceType::Rook);
        case 'b':
            return make_piece(color, chesslib::PieceType::Bishop);
        case 'n':
            return make_piece(color, chesslib::PieceType::Knight);
        case 'p':
            return make_piece(color, chesslib::PieceType::Pawn);
        
        default:
            return chesslib::kEmpty;
        } 
    }
} // namespace

namespace chesslib
{

Board Board::start_position() {
    return from_fen(std::string_view("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
}

Board Board::from_fen(std::string_view fen) {
    Board b;
    b.cells_ = make_empty();

    std::array<std::string_view, 6> fields;
    int field_num = 0;
    int last = 0;
    int i = 0;
    for (char c: fen) {
        if (c == ' ') {
            if (i != last)
                fields.at(field_num++) = std::string_view(fen.data() + last, i - last);
            last = i + 1;
            if (field_num > 6)
                throw std::invalid_argument("from_fen: fen string has to many fields");
        }
        i++;
    }
    if (i != last)
        fields.at(field_num++) = std::string_view(fen.data() + last, i - last);

    if (field_num < 6) {
        throw std::invalid_argument("from_fen: fen string has to few fields");
    }
    
    std::string_view placement = fields[0];

    int rank = 7;
    int file = 0;
    for (char c: placement) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if ('0' <= c && c <= '9') {
            if (c == '9')
                throw std::invalid_argument("from_fen: bad empty spaces number");
            file += c - '0';
        } else {
            b.cells_[rank * 8 + file++] = piece_from_char(c);
        }
    }

    if (fields[1][0] == 'b')
        b.side_ = Color::Black;
    else if (fields[1][0] == 'w')
        b.side_ = Color::White;
    else
        throw std::invalid_argument("from_fen: incorrect color");
    
    for (char c: fields[2]) {
        switch (c)
        {
        case 'q':
            b.castling_ |= 1;
            break;
        case 'k':
            b.castling_ |= 1 << 1;
            break;
        case 'Q':
            b.castling_ |= 1 << 2;
            break;
        case 'K':
            b.castling_ |= 1 << 3;
            break;
        case '-':
            break;
        
        default:
            throw std::invalid_argument("from_fen: bad castling char");
        }
    }

    if (fields[3][0] != '-') 
        b.ep_file_ = fields[3][0] - '0';
    
    b.halfmove_clock_ = atoi(fields[5].data());

    return b;
}

std::string Board::to_fen() const noexcept {
    return "";
}

Board Board::make_move(Move m) const {return Board{};}

} // namespace chesslib