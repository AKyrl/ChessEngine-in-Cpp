#include "PrincipalVariation.hpp"
#include "PrincipalVariation.hpp"
#include "PrincipalVariation.hpp"

#include <ostream>


bool PrincipalVariation::isMate() const {
    return endInMate;
}

int PrincipalVariation::score() const {
    if (isMate())
        return (int)length();
    else 
        return endScore;
}

void PrincipalVariation::setMate( bool mate)
{
    endInMate = mate;
}

void PrincipalVariation::setScore( int score)
{
    endScore = score;
}

std::size_t PrincipalVariation::length() const {
    return pvMoves.size();
}

PrincipalVariation::MoveIter PrincipalVariation::begin() const {
    return pvMoves.data();
}

PrincipalVariation::MoveIter PrincipalVariation::end() const {
   // auto last = pvMoves.data()[pvMoves.size()]; // get last object of vector
    return &pvMoves.data()[pvMoves.size()];
}

std::ostream& operator<<(std::ostream& os, const PrincipalVariation& pv) {
   
    for (auto it = pv.begin(); it != pv.end(); it++) {
        os << *it << ' ';
    }

    return os;
}
