#include "Piece.hpp"

#include <ostream>

Piece::Piece(PieceColor color, PieceType type) : piece_color(color), piece_type(type)
{
    //piece_color = color;
    //piece_type = type;
}

Piece::Optional Piece::fromSymbol(char symbol) {
    switch (symbol) {
    case 'p':
        return Piece(PieceColor::Black, PieceType::Pawn);
    case 'n':
        return Piece(PieceColor::Black, PieceType::Knight);
    case 'b':
        return Piece(PieceColor::Black, PieceType::Bishop);
    case 'r':
        return Piece(PieceColor::Black, PieceType::Rook);
    case 'q':
        return Piece(PieceColor::Black, PieceType::Queen);
    case 'k':
        return Piece(PieceColor::Black, PieceType::King);
    case 'P':
        return Piece(PieceColor::White, PieceType::Pawn);
    case 'N':
        return Piece(PieceColor::White, PieceType::Knight);
    case 'B':
        return Piece(PieceColor::White, PieceType::Bishop);
    case 'R':
        return Piece(PieceColor::White, PieceType::Rook);
    case 'Q':
        return Piece(PieceColor::White, PieceType::Queen);
    case 'K':
        return Piece(PieceColor::White, PieceType::King);
    default:
        return std::nullopt;
    }
    
}

PieceColor Piece::color() const {
    return piece_color;
}

PieceType Piece::type() const {
    return piece_type;
}

bool operator==(const Piece& lhs, const Piece& rhs) {
    if (lhs.color() == rhs.color() && lhs.type() == rhs.type() )
        return true;
    else
        return false;
}

std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    PieceColor c = piece.color();
    PieceType t = piece.type();
    
    if (c == PieceColor::White)
    {
        switch (t)
        {
        case PieceType::Pawn:
            os << 'P';
            break;
        case PieceType::Knight:
            os << 'N';
            break;
        case PieceType::Bishop:
            os << 'B';
            break;
        case PieceType::Rook:
            os << 'R';
            break;
        case PieceType::Queen:
            os << 'Q';
            break;
        case PieceType::King:
            os << 'K';
            break;
        default:
            os << '?';
            break;
        }
    }
    else
    {
        switch (t)
        {
        case PieceType::Pawn:
            os << 'p';
            break;
        case PieceType::Knight:
            os << 'n';
            break;
        case PieceType::Bishop:
            os << 'b';
            break;
        case PieceType::Rook:
            os << 'r';
            break;
        case PieceType::Queen:
            os << 'q';
            break;
        case PieceType::King:
            os << 'k';
            break;
        default:
            os << '?';
            break;
        }
    }
    return os;
}

PieceColor operator!(PieceColor color) {
    if (color == PieceColor::White)
        return PieceColor::Black;
    else
        return PieceColor::White;
}
