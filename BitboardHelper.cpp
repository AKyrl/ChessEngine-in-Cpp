#include "BitboardHelper.hpp"


void setBit(uint64_t& bitboard, int index) {
    bitboard |= 1ULL << (index);
    return;
}


bool getBit(uint64_t bitboard, int index) {
    return  bitboard & (1ULL << (index));
}


void popBit(uint64_t& bitboard, int index) {
    bitboard &= ~(1ULL << (index));
    return;
}

int getBitboardIndex(const Piece::Optional& piece) {
    if (!piece.has_value())
        return 69;
    else if (piece.value().color() == PieceColor::White) {
        PieceType type = piece.value().type();
        switch (type)
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
        }
    }
    else
    {
        PieceType type = piece.value().type();
        switch (type)
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
        }
    }
    return 69;
}


Piece::Optional getPieceFromIndex(int index) {
    switch (index)
    {
    case 0:
        return Piece(PieceColor::White, PieceType::Pawn);
    case 1:
        return Piece(PieceColor::White, PieceType::Knight);
    case 2:
        return Piece(PieceColor::White, PieceType::Bishop);
    case 3:
        return Piece(PieceColor::White, PieceType::Rook);
    case 4:
        return Piece(PieceColor::White, PieceType::Queen);
    case 5:
        return Piece(PieceColor::White, PieceType::King);
    case 6:
        return Piece(PieceColor::Black, PieceType::Pawn);
    case 7:
        return Piece(PieceColor::Black, PieceType::Knight);
    case 8:
        return Piece(PieceColor::Black, PieceType::Bishop);
    case 9:
        return Piece(PieceColor::Black, PieceType::Rook);
    case 10:
        return Piece(PieceColor::Black, PieceType::Queen);
    case 11:
        return Piece(PieceColor::Black, PieceType::King);
    default:
        return std::nullopt;
    }
}


int getOccupanciesIndex(const Piece::Optional& piece) {
    if (!piece.has_value())
        return 69;
    else if (piece.value().color() == PieceColor::White)
        return White;
    else
        return Black;
}

int countPiecies(uint64_t bitboard)
{
    int cntr = 0;
    while (bitboard)
    {
        cntr++;
        bitboard &= bitboard - 1;
    }

    return cntr;
}

//int getLSPawnIndex(uint64_t bitboard)
//{
//    if (bitboard)
//        return countPiecies((bitboard & -bitboard) - 1);
//    else
//        return -1;
//}

const int lsb_64_table[64] =
{
   63, 30,  3, 32, 59, 14, 11, 33,
   60, 24, 50,  9, 55, 19, 21, 34,
   61, 29,  2, 53, 51, 23, 41, 18,
   56, 28,  1, 43, 46, 27,  0, 35,
   62, 31, 58,  4,  5, 49, 54,  6,
   15, 52, 12, 40,  7, 42, 45, 16,
   25, 57, 48, 13, 10, 39,  8, 44,
   20, 47, 38, 22, 17, 37, 36, 26
};

// Matt Taylor's folding trick
int getLSPawnIndex(uint64_t bb) {
    unsigned int folded;
    if (bb) {
        bb ^= bb - 1;
        folded = (int)bb ^ (bb >> 32);
        return lsb_64_table[folded * 0x78291ACF >> 26];
    }
    else
        return -1;
    
}
