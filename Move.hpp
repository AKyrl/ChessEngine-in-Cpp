#ifndef CHESS_ENGINE_MOVE_HPP
#define CHESS_ENGINE_MOVE_HPP

#include "Square.hpp"
#include "Piece.hpp"

#include <iosfwd>
#include <optional>
#include <string>

class Move {
    Square mvFrom, mvTo;
    std::optional<PieceType> mvPromotion = std::nullopt;
    int moveScore;

public:

    using Optional = std::optional<Move>;

    Move();
    Move(const Square& from, const Square& to, 
         const std::optional<PieceType>& promotion = std::nullopt, int score = 0);

    static Optional fromUci(const std::string& uci);

    Square from() const;
    Square to() const;
    std::optional<PieceType> promotion() const;

    int score() const;
    void setScore(int s);
    
};

std::ostream& operator<<(std::ostream& os, const Move& move);

// Needed for std::map, std::set
bool operator<(const Move& lhs, const Move& rhs);
bool operator>(const Move& lhs, const Move& rhs);
bool operator==(const Move& lhs, const Move& rhs);


#endif
