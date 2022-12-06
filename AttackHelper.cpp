#include "AttackHelper.hpp"

uint64_t rookAttacks[64][4096];
uint64_t bishopAttacks[64][512];

Attacks::Attacks() {
    //initalise Leaper piecies
    for (int square = 0; square < 64; square++)
    {
        pawnAttacks[White][square] = getPawnAttacksLookup(PieceColor::White, square);
        pawnAttacks[Black][square] = getPawnAttacksLookup(PieceColor::Black, square);

        knightAttacks[square] = getKnightAttacksLookup(square);
        kingAttacks[square] = getKingAttacksLookup(square);
    

    //initalise Sliding piecies
        bishopMasks[square] = getBishopMask(square);
        rookMasks[square] = getRookMask(square);

        uint64_t bishopMask = bishopMasks[square];
        int bishopCount = countPiecies(bishopMask);
        int bishopOccupancyIndecies = (1 << bishopCount);

        for (int index = 0; index < bishopOccupancyIndecies; index++)
        {
            uint64_t occupancy = getOccupancyVariation(index, bishopCount, bishopMask);
            int magic_index = int( (occupancy * bishopMagicNumbers[square]) >> (64 - bishopBits[square]) );
            bishopAttacks[square][magic_index] = getBishopMask(square, occupancy);
        }
        
        uint64_t rookMask = rookMasks[square];
        int rookCount = countPiecies(rookMask);
        int rookOccupancyIndecies = (1 << rookCount);

        for (int index = 0; index < rookOccupancyIndecies; index++)
        {  
            uint64_t occupancy = getOccupancyVariation(index, rookCount, rookMask);
            int magic_index = int( (occupancy * rookMagicNumbers[square]) >> (64 - rookBits[square]) );
            rookAttacks[square][magic_index] = getRookMask(square, occupancy);
        }
    }
}

uint64_t Attacks::getPawnAttacksLookup(PieceColor side, int square)
{
    uint64_t attacks = 0ULL;
    uint64_t bitboard = 0ULL;

    setBit(bitboard, square);

    if (side == PieceColor::Black)
    {
        if ((bitboard >> 7) & not_A_file) 
            attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_H_file) 
            attacks |= (bitboard >> 9);
    }
    else
    {
        if ((bitboard << 7) & not_H_file)
            attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_A_file)
            attacks |= (bitboard << 9);
    }

    return attacks;

}

uint64_t Attacks::getKnightAttacksLookup(int square)
{
    uint64_t attacks = 0ULL;
    uint64_t bitboard = 0ULL;
    setBit(bitboard, square);

    if ((bitboard >> 17) & not_H_file) 
        attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_A_file) 
        attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_GH_file) 
        attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_AB_file) 
        attacks |= (bitboard >> 6);
    if ((bitboard << 17) & not_A_file) 
        attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_H_file) 
        attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_AB_file) 
        attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_GH_file) 
        attacks |= (bitboard << 6);

    return attacks;
}

uint64_t Attacks::getKingAttacksLookup(int square)
{
    uint64_t attacks = 0ULL;
    uint64_t bitboard = 0ULL;
    setBit(bitboard, square);

    if (bitboard >> 8) 
        attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & not_H_file) 
        attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & not_A_file)
        attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & not_H_file) 
        attacks |= (bitboard >> 1);
    if (bitboard << 8) 
        attacks |= (bitboard << 8);
    if ((bitboard << 9) & not_A_file) 
        attacks |= (bitboard << 9);
    if ((bitboard << 7) & not_H_file)
        attacks |= (bitboard << 7);
    if ((bitboard << 1) & not_A_file) 
        attacks |= (bitboard << 1);

    return attacks;
}

uint64_t Attacks::getBishopMask(int square)
{
    uint64_t attacks = 0ULL;
    int rank, file;

    int directionRank = square / 8;
    int directionFile = square % 8;

    for (rank = directionRank + 1, file = directionFile + 1; rank <= 6 && file <= 6; rank++, file++) 
        attacks |= (1ULL << (rank * 8 + file));

    for (rank = directionRank - 1, file = directionFile + 1; rank >= 1 && file <= 6; rank--, file++) 
        attacks |= (1ULL << (rank * 8 + file));

    for (rank = directionRank + 1, file = directionFile - 1; rank <= 6 && file >= 1; rank++, file--) 
        attacks |= (1ULL << (rank * 8 + file));

    for (rank = directionRank - 1, file = directionFile - 1; rank >= 1 && file >= 1; rank--, file--) 
        attacks |= (1ULL << (rank * 8 + file));

    return attacks;
}

uint64_t Attacks::getRookMask(int square)
{
    uint64_t attacks = 0ULL;
    int rank, file;

    int directionRank = square / 8;
    int directionFile = square % 8;

    for (rank = directionRank + 1; rank <= 6; rank++)
        attacks |= (1ULL << (rank * 8 + directionFile));

    for (rank = directionRank - 1; rank >= 1; rank--) 
        attacks |= (1ULL << (rank * 8 + directionFile));

    for (file = directionFile + 1; file <= 6; file++) 
        attacks |= (1ULL << (directionRank * 8 + file));

    for (file = directionFile - 1; file >= 1; file--) 
        attacks |= (1ULL << (directionRank * 8 + file));

    return attacks;
}

uint64_t Attacks::getBishopMask(int square, uint64_t blockers)
{
    uint64_t attacks = 0ULL;
    int rank, file;

    int directionRank = square / 8;
    int directionFile = square % 8;

    for (rank = directionRank + 1, file = directionFile + 1; rank <= 7 && file <= 7; rank++, file++)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & blockers) 
            break;
    }

    for (rank = directionRank - 1, file = directionFile + 1; rank >= 0 && file <= 7; rank--, file++)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & blockers) 
            break;
    }

    for (rank = directionRank + 1, file = directionFile - 1; rank <= 7 && file >= 0; rank++, file--)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & blockers) 
            break;
    }

    for (rank = directionRank - 1, file = directionFile - 1; rank >= 0 && file >= 0; rank--, file--)
    {
        attacks |= (1ULL << (rank * 8 + file));
        if ((1ULL << (rank * 8 + file)) & blockers) 
            break;
    }

    return attacks;
}

uint64_t Attacks::getRookMask(int square, uint64_t blockers)
{
    uint64_t attacks = 0ULL;
    int rank, file;

    int directionRank = square / 8;
    int directionFile = square % 8;

    for (rank = directionRank + 1; rank <= 7; rank++)
    {
        attacks |= (1ULL << (rank * 8 + directionFile));
        if ((1ULL << (rank * 8 + directionFile)) & blockers) 
            break;
    }

    for (rank = directionRank - 1; rank >= 0; rank--)
    {
        attacks |= (1ULL << (rank * 8 + directionFile));
        if ((1ULL << (rank * 8 + directionFile)) & blockers) 
            break;
    }

    for (file = directionFile + 1; file <= 7; file++)
    {
        attacks |= (1ULL << (directionRank * 8 + file));
        if ((1ULL << (directionRank * 8 + file)) & blockers) 
            break;
    }

    for (file = directionFile - 1; file >= 0; file--)
    {
        attacks |= (1ULL << (directionRank * 8 + file));
        if ((1ULL << (directionRank * 8 + file)) & blockers) 
            break;
    }

    return attacks;
}

uint64_t Attacks::getBishopAttacks(int square, uint64_t occupancies) const
{
    occupancies &= bishopMasks[square];
    occupancies *= bishopMagicNumbers[square];
    occupancies >>= 64 - bishopBits[square];

    return bishopAttacks[square][occupancies];
}

uint64_t Attacks::getRookAttacks(int square, uint64_t occupancies) const
{
    occupancies &= rookMasks[square];
    occupancies *= rookMagicNumbers[square];
    occupancies >>= 64 - rookBits[square];

    return rookAttacks[square][occupancies];
}

uint64_t Attacks::getQueenAttacks(int square, uint64_t occupancies) const
{
    return getBishopAttacks(square, occupancies) | getRookAttacks(square, occupancies);
}


uint64_t Attacks::getOccupancyVariation(int index, int bits_in_mask, uint64_t attack_mask)
{
    uint64_t occupancy = 0ULL;

    // loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; count++)
    {
        int square = getLSPawnIndex(attack_mask);
        popBit(attack_mask, square);

        //check if on board
        if (index & (1 << count))
            occupancy |= (1ULL << square);
    }

    return occupancy;
}

