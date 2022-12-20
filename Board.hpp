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

struct BoardCoppy
{
    uint64_t backup_bitboards[12];
    uint64_t backup_occupancies[3];
    PieceColor backup_boardTurn;
    CastlingRights backup_boardCastlingRights;
    Square::Optional backup_boardEnpassantSquare;
};

class Board {
private:
    // white        black
    // P N B R Q K  p n b r q k  
    uint64_t bitboards[12]; 

    // 0=white 1=black 2=all
    uint64_t occupancies[3];

    PieceColor boardTurn;
    CastlingRights boardCastlingRights;
    Square::Optional boardEnpassantSquare;

    Attacks boardAttacks;

    //For restoring after move
    /*uint64_t backup_bitboards[12];
    uint64_t backup_occupancies[3];
    PieceColor backup_boardTurn;
    CastlingRights backup_boardCastlingRights;
    Square::Optional backup_boardEnpassantSquare;*/

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
    bool isKingInCheck();

    // add a move to the Vec if its legal
    void addMoveIfLegal(Board::MoveVec& moves, Move move);

    // 0 = earlygame, -1 = endgame, other  = mid
    int getGamePhase();

    int evaluate();

    BoardCoppy storeBoard();
    void restoreBoard(BoardCoppy cp);
};



std::ostream& operator<<(std::ostream& os, const Board& board);

#endif
