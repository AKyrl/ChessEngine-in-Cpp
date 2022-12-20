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

int Piece::toInt()
{
    PieceColor c = color();
    PieceType t = type();
    
    if (c == PieceColor::White)
    {
        switch (t)
        {
        case PieceType::Pawn:
            return 0;
        case PieceType::Knight:
            return 1;
        case PieceType::Bishop:
            return 2;
        case PieceType::Rook:
            return 3;
        case PieceType::Queen:
            return 4;
        case PieceType::King:
            return 5;
        default:
            return 69;
        }
    }
    else
    {
        switch (t)
        {
        case PieceType::Pawn:
            return 6;
        case PieceType::Knight:
            return 7;
        case PieceType::Bishop:
            return 8;
        case PieceType::Rook:
            return 9;
        case PieceType::Queen:
            return 10;
        case PieceType::King:
            return 11;
        default:
            return 69;
        }
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

//create piece type ordering
int rank(PieceType piece)
{
    switch (piece)
    {
    case PieceType::Pawn:
        return 1;
        break;
    case PieceType::Knight:
        return 3;
        break;
    case PieceType::Bishop:
        return 4; //actual value is 3 upgraded for different rank
        break;
    case PieceType::Rook:
        return 5;
        break;
    case PieceType::Queen:
        return 9;
        break;
    case PieceType::King:
        return 10; //normally does not have a rank
        break;
    default:
        return 0;
    }
}

//bool operator<(const PieceType& lhs, const PieceType& rhs) {
//    if (rank(lhs) < rank(rhs))
//        return true;
//    else
//        return false;
//}

std::ostream& operator<<(std::ostream& os, const PieceColor& color) {
    if (color == PieceColor::White)
        os << "White";
    else if (color == PieceColor::Black)
        os << "Black";
    else
        os << "?";
    return os;
}

std::ostream& operator<<(std::ostream& os, const PieceType& type) {
    
    switch (type)
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

    return os;
}