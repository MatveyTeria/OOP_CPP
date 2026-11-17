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

constexpr uint64_t splitmix64(uint64_t& state) noexcept {
    uint64_t z = (state += 0x9E3779B97F4A7C15ull);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);
}

// piece_square[12] — это kEmpty; оставляем нули, чтобы XOR был no-op.
constexpr std::array<std::array<uint64_t, 64>, 13> piece_square = [] {
    std::array<std::array<uint64_t, 64>, 13> t{};
    uint64_t s = 0xCAFEBABE12345678ull;
    for (int p = 0; p < 12; ++p)
        for (int sq = 0; sq < 64; ++sq)
            t[p][sq] = splitmix64(s);
    return t;
}();

constexpr std::array<uint64_t, 8> ep_file = [] {
    std::array<uint64_t, 8> t{};
    uint64_t s = 0xDEADBEEFDEADBEEFull;
    for (int f = 0; f < 8; ++f) t[f] = splitmix64(s);
    return t;
}();

constexpr uint64_t side = 0x9E3779B97F4A7C15ull;

constexpr std::array<uint64_t, 16> castling = [] {
    std::array<uint64_t, 4> bit{};
    uint64_t s = 0x1234567890ABCDEFull;
    for (int i = 0; i < 4; ++i) bit[i] = splitmix64(s);

    std::array<uint64_t, 16> t{};
    for (int c = 0; c < 16; ++c)
        for (int i = 0; i < 4; ++i)
            if (c & (1 << i)) t[c] ^= bit[i];
    return t;
}();

} // namespace zobrist

#include "chesslib/Board.h"
#include <cassert>
#include <stdexcept>

namespace { /* piece_from_char ... без изменений */ }

namespace zobrist { /* см. выше */ }

namespace chesslib {

Board Board::start_position() {
    return from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board Board::from_fen(std::string_view fen) {
    Board b;
    b.cells_ = make_empty();

    // ---- разбиение на поля ----
    std::array<std::string_view, 6> fields{};
    int field_num = 0, last = 0, i = 0;
    for (char c : fen) {
        if (c == ' ') {
            if (i != last) {
                if (field_num >= 6)
                    throw std::invalid_argument("from_fen: too many fields");
                fields[field_num++] = std::string_view(fen.data() + last, i - last);
            }
            last = i + 1;
        }
        i++;
    }
    if (i != last) {
        if (field_num >= 6)
            throw std::invalid_argument("from_fen: too many fields");
        fields[field_num++] = std::string_view(fen.data() + last, i - last);
    }
    if (field_num < 6)
        throw std::invalid_argument("from_fen: too few fields");

    // ---- расстановка ----
    int rank = 7, file = 0;
    for (char c : fields[0]) {
        if (c == '/') {
            if (file != 8) throw std::invalid_argument("from_fen: bad rank length");
            --rank;
            if (rank < 0)  throw std::invalid_argument("from_fen: too many ranks");
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += c - '0';
            if (file > 8) throw std::invalid_argument("from_fen: rank overflow");
        } else {
            if (file >= 8) throw std::invalid_argument("from_fen: rank overflow");
            b.cells_[rank * 8 + file++] = piece_from_char(c);
        }
    }
    if (rank != 0 || file != 8)
        throw std::invalid_argument("from_fen: bad placement");

    // ---- сторона ----
    if      (fields[1] == "w") b.side_ = Color::White;
    else if (fields[1] == "b") b.side_ = Color::Black;
    else throw std::invalid_argument("from_fen: bad side");

    // ---- рокировки ----
    if (fields[2] != "-") {
        for (char c : fields[2]) {
            switch (c) {
                case 'q': b.castling_ |= Castle::BlackQueen; break;
                case 'k': b.castling_ |= Castle::BlackKing;  break;
                case 'Q': b.castling_ |= Castle::WhiteQueen; break;
                case 'K': b.castling_ |= Castle::WhiteKing;  break;
                default: throw std::invalid_argument("from_fen: bad castling");
            }
        }
    }

    // ---- en passant ----
    if (fields[3] != "-") {
        if (fields[3].size() != 2 ||
            fields[3][0] < 'a' || fields[3][0] > 'h' ||
            (fields[3][1] != '3' && fields[3][1] != '6'))
            throw std::invalid_argument("from_fen: bad en passant");
        b.ep_file_ = int8_t(fields[3][0] - 'a');
    }

    // ---- halfmove clock (без atoi: string_view не нуль-терминирован) ----
    int hc = 0;
    for (char c : fields[4]) {
        if (c < '0' || c > '9')
            throw std::invalid_argument("from_fen: bad halfmove clock");
        hc = hc * 10 + (c - '0');
        if (hc > 255) throw std::invalid_argument("from_fen: halfmove overflow");
    }
    b.halfmove_clock_ = uint8_t(hc);

    // ---- ГЛАВНОЕ: установить zobrist, иначе инвариант нарушен ----
    b.zobrist_ = b.recompute_zobrist();
    return b;
}

std::string Board::to_fen() const noexcept {
    std::string fen;
    fen.reserve(90);

    // 1. Расстановка, от 8-й горизонтали к 1-й
    for (int r = 7; r >= 0; --r) {
        int empty = 0;
        for (int f = 0; f < 8; ++f) {
            uint8_t p = cells_[r * 8 + f];
            if (is_empty(p)) { ++empty; continue; }
            if (empty) { fen += char('0' + empty); empty = 0; }
            char c;
            switch (type_of(p)) {
                case PieceType::Pawn:   c = 'p'; break;
                case PieceType::Knight: c = 'n'; break;
                case PieceType::Bishop: c = 'b'; break;
                case PieceType::Rook:   c = 'r'; break;
                case PieceType::Queen:  c = 'q'; break;
                case PieceType::King:   c = 'k'; break;
            }
            if (color_of(p) == Color::White) c = char(c - 'a' + 'A');
            fen += c;
        }
        if (empty) fen += char('0' + empty);
        if (r > 0) fen += '/';
    }

    // 2. Сторона
    fen += ' ';
    fen += (side_ == Color::White) ? 'w' : 'b';

    // 3. Рокировки
    fen += ' ';
    if (castling_ == 0) {
        fen += '-';
    } else {
        if (castling_ & Castle::WhiteKing)  fen += 'K';
        if (castling_ & Castle::WhiteQueen) fen += 'Q';
        if (castling_ & Castle::BlackKing)  fen += 'k';
        if (castling_ & Castle::BlackQueen) fen += 'q';
    }

    // 4. En passant.
    fen += ' ';
    if (ep_file_ == -1) {
        fen += '-';
    } else {
        const int tr = (side_ == Color::White) ? 5 : 2;
        fen += char('a' + ep_file_);
        fen += char('1' + tr);
    }

    // 5. Halfmove clock
    fen += ' ';
    fen += std::to_string(static_cast<unsigned>(halfmove_clock_));

    // 6. Fullmove number — не храним, выводим 1
    fen += " 1";
    return fen;
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