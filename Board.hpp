#ifndef CHESS_ENGINE_BOARD_HPP
#define CHESS_ENGINE_BOARD_HPP

#include "Piece.hpp"
#include "Square.hpp"
#include "Move.hpp"
#include "CastlingRights.hpp"
#include "AttackHelper.hpp"
#include "EvaluationHelper.hpp"

#include <optional>
#include <iosfwd>
#include <vector>

class Board {
private:
    // white        black
    // P N B R Q K  p n b r q k  
    uint64_t bitboards[12]; 

    // 0=white 1=black 2=all
    uint64_t occupancies[3];

    PieceColor boardTurn;
    CastlingRights bordCastlingRights;
    Square::Optional boardEnpassantSquare;

    Attacks boardAttacks;

public:


    using Optional = std::optional<Board>;
    using MoveVec = std::vector<Move>;

    //Board::MoveVec getKnightMoves();

    Board();

    void setPiece(const Square& square, const Piece::Optional& piece);
    void removePiece(const Square& square, Piece piece);
    Piece::Optional piece(const Square& square) const;
    void setTurn(PieceColor turn);
    PieceColor turn() const;
    void setCastlingRights(CastlingRights cr);
    CastlingRights castlingRights() const;
    void setEnPassantSquare(const Square::Optional& square);
    Square::Optional enPassantSquare() const;

    void makeMove(const Move& move);

    void pseudoLegalMoves(MoveVec& moves);
    void pseudoLegalMovesFrom(const Square& from, MoveVec& moves);

    // check if a square is attacked by given side
    bool isSquareAttacked(int square, PieceColor side) const;
    // add a move to the Vec if its legal
    void addMoveIfLegal(Board::MoveVec& moves, Move move);

    // 0 = earlygame, -1 = endgame, other  = mid
    int getGamePhase();

    int evaluate();
};



std::ostream& operator<<(std::ostream& os, const Board& board);

#endif
