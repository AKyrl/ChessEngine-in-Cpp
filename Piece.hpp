#ifndef CHESS_ENGINE_PIECE_HPP
#define CHESS_ENGINE_PIECE_HPP

#include <optional>
#include <iosfwd>

enum class PieceColor {
    White,
    Black
};

enum class PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

//bool operator<(const PieceType& lhs, const PieceType& rhs);

class Piece {
    PieceColor piece_color;
    PieceType piece_type;
public:

    using Optional = std::optional<Piece>;

    Piece(PieceColor color, PieceType type);

    static Optional fromSymbol(char symbol);

    int toInt();

    PieceColor color() const;
    PieceType type() const;
};

bool operator==(const Piece& lhs, const Piece& rhs);
std::ostream& operator<<(std::ostream& os, const Piece& piece);

// Invert a color (White becomes Black and vice versa)
PieceColor operator!(PieceColor color);

std::ostream& operator<<(std::ostream& os, const PieceType& piece);
std::ostream& operator<<(std::ostream& os, const PieceColor& color);

#endif
