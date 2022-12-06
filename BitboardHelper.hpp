#ifndef CHESS_ENGINE_BITBOARDHELPER_HPP
#define CHESS_ENGINE_BITBOARDHELPER_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"

#include <optional>
#include <iosfwd>
#include <vector>


enum {
	White = 0,
	Black = 1,
	Both = 2
};

enum {
	P = 0,
	N = 1, 
	B = 2, 
	R = 3, 
	Q = 4, 
	K = 5,  
	p = 6, 
	n = 7, 
	b = 8, 
	r = 9, 
	q = 10, 
	k = 11
};
// set index bit in bitboard as 1
void setBit(uint64_t& bitboard, int index);

// get value of bit in index
bool getBit(uint64_t bitboard, int index);

// set inex bit in bitboard as 0
void popBit(uint64_t& bitboard, int index);

// get biboard index for piece 0-11 (W/B - p n b r q k)
int getBitboardIndex(const Piece::Optional& piece);

// occupancies for white/black 0 1
int getOccupanciesIndex(const Piece::Optional& piece);

// get pice type and color from index 0-11 (W/B - p n b r q k)
Piece::Optional getPieceFromIndex(int index);

// get the number of piecies on the bitboard
int countPiecies(uint64_t bitboard);

//get the Index of the least significant pawn in the bitboard
int getLSPawnIndex(uint64_t bitboard);

#endif