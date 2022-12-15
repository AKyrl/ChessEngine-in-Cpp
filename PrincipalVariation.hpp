#ifndef CHESS_ENGINE_PRINCIPALVARIATION_HPP
#define CHESS_ENGINE_PRINCIPALVARIATION_HPP

#include "Move.hpp"
#include "Piece.hpp"

#include <iosfwd>
#include <cstddef>
#include <vector>

class PrincipalVariation {

    int endScore = 0;
    bool endInMate = false;

public:

    using MoveIter = const Move*;

    std::vector<Move> pvMoves;

    bool isMate() const;
    int score() const;

    void setMate( bool mate);
    void setScore( int score);

    std::size_t length() const;
    MoveIter begin() const;
    MoveIter end() const;
};

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv);

#endif
