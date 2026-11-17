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

namespace zobrist {
    inline const std::array<std::array<uint64_t, 64>, 13> piece_square = /* [12][*] == 0 */;
    inline const std::array<uint64_t, 8>  ep_file  = /* ... */;
    inline constexpr uint64_t side = 0x9E3779B97F4A7C15ull;  // любое фиксированное

    // castling[c] = XOR хэшей отдельных битов, входящих в c
    inline const std::array<uint64_t, 16> castling = []{
        std::array<uint64_t, 4> bit{ 0x..., 0x..., 0x..., 0x... };
        std::array<uint64_t, 16> t{};
        for (int c = 0; c < 16; ++c)
            for (int i = 0; i < 4; ++i)
                if (c & (1 << i)) t[c] ^= bit[i];
        return t;
    }();
}

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
        b.ep_file_ = fields[3][0] - 'a';
    
    b.halfmove_clock_ = atoi(fields[4].data());

    return b;
}

std::string Board::to_fen() const noexcept {
    return "";
}

Board Board::make_move(Move m) const noexcept {
    Board b = *this;
    const Color us   = b.side_;
    const Color them = (us == Color::White) ? Color::Black : Color::White;

    const uint8_t moving   = b.cells_[m.from];
    const uint8_t captured = b.cells_[m.to];   // kEmpty, если взятия на m.to нет (в т.ч. en passant)

    // ---- (A) XOR-out старого состояния ----
    // piece_square[kEmpty][*] == 0 — XOR с пустотой = no-op, ветка if не нужна
    b.zobrist_ ^= zobrist::piece_square[moving][m.from];
    if (captured != kEmpty)
        b.zobrist_ ^= zobrist::piece_square[captured][m.to];
    b.zobrist_ ^= zobrist::castling[b.castling_];
    if (b.ep_file_ != -1) b.zobrist_ ^= zobrist::ep_file[b.ep_file_];
    // side_hash — НЕ здесь: сторона меняется «тогглом», см. шаг (G)

    // ---- (B) En passant: снять пешку позади ----
    if (m.type == MoveType::EnPassant) {
        const Square cap = (us == Color::White) ? Square(m.to - 8) : Square(m.to + 8);
        b.zobrist_ ^= zobrist::piece_square[b.cells_[cap]][cap];
        b.cells_[cap] = kEmpty;
    }

    // ---- (C) Castling: переместить ладью ----
    if (m.type == MoveType::Castle) {
        const int    r  = rank_of(m.to);
        const bool   ks = (file_of(m.to) == 6);
        const Square rf = make_square(ks ? 7 : 0, r);
        const Square rt = make_square(ks ? 5 : 3, r);
        const uint8_t rook = b.cells_[rf];
        b.zobrist_ ^= zobrist::piece_square[rook][rf];
        b.zobrist_ ^= zobrist::piece_square[rook][rt];
        b.cells_[rf] = kEmpty;
        b.cells_[rt] = rook;
    }

    // ---- (D) Переместить саму фигуру (с превращением) ----
    b.cells_[m.from] = kEmpty;
    const uint8_t new_piece = (m.type == MoveType::Promotion)
        ? make_piece(us, m.promo) : moving;
    b.cells_[m.to] = new_piece;

    // ---- (E) Права рокировки ----
    if (type_of(moving) == PieceType::King) {
        b.castling_ &= (us == Color::White)
            ? static_cast<uint8_t>(~(Castle::WhiteKing | Castle::WhiteQueen))
            : static_cast<uint8_t>(~(Castle::BlackKing | Castle::BlackQueen));
    }
    if (m.from == 0)  b.castling_ &= ~uint8_t(Castle::WhiteQueen);  // a1
    if (m.from == 7)  b.castling_ &= ~uint8_t(Castle::WhiteKing);   // h1
    if (m.from == 56) b.castling_ &= ~uint8_t(Castle::BlackQueen);  // a8
    if (m.from == 63) b.castling_ &= ~uint8_t(Castle::BlackKing);   // h8
    if (m.to   == 0)  b.castling_ &= ~uint8_t(Castle::WhiteQueen);  // взята ладья a1
    if (m.to   == 7)  b.castling_ &= ~uint8_t(Castle::WhiteKing);   // взята ладья h1
    if (m.to   == 56) b.castling_ &= ~uint8_t(Castle::BlackQueen);
    if (m.to   == 63) b.castling_ &= ~uint8_t(Castle::BlackKing);

    // ---- (F) ep_file для следующего хода ----
    const int rd = rank_of(m.to) - rank_of(m.from);
    b.ep_file_ = (type_of(moving) == PieceType::Pawn && (rd == 2 || rd == -2))
        ? static_cast<int8_t>(file_of(m.to)) : -1;

    // ---- (G) Halfmove clock ----
    const bool is_capture = !is_empty(captured) || m.type == MoveType::EnPassant;
    const bool is_pawn    = type_of(moving) == PieceType::Pawn;
    if (is_capture || is_pawn) b.halfmove_clock_ = 0;
    else if (b.halfmove_clock_ < 255) b.halfmove_clock_++;

    // ---- (H) Смена стороны ----
    b.side_ = them;

    // ---- (I) XOR-in нового состояния ----
    b.zobrist_ ^= zobrist::piece_square[new_piece][m.to];
    b.zobrist_ ^= zobrist::castling[b.castling_];
    if (b.ep_file_ != -1) b.zobrist_ ^= zobrist::ep_file[b.ep_file_];
    b.zobrist_ ^= zobrist::side;   // toggle: сторона всегда меняется ровно на 1 XOR

    return b;
}

uint64_t Board::recompute_zobrist() const noexcept {
    uint64_t h = 0;
    for (int sq = 0; sq < 64; ++sq)
        h ^= zobrist::piece_square[cells_[sq]][sq];
    h ^= zobrist::castling[castling_];
    if (ep_file_ != -1) h ^= zobrist::ep_file[ep_file_];
    if (side_ == Color::Black) h ^= zobrist::side;
    return h;
}

} // namespace chesslib