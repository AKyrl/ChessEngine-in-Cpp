#include "Move.hpp"

Move::Move() : mvFrom(Square::A1), mvTo(Square::A1)
{
     
}

Move::Move(const Square& from, const Square& to, 
           const std::optional<PieceType>& promotion, int score) :mvFrom(from), mvTo(to)//, mvPromotion(promotion), moveScore(score)

{
    // mvFrom = from.fromIndex(from.index()).value();
    // mvTo = to.fromIndex(to.index()).value();
    mvPromotion = promotion;
    moveScore = score;
    
}

Move::Optional Move::fromUci(const std::string& uci) {
    std::string strFrom, strTo;
    char strPromotion;
    if (uci.length() == 5)
    {
        strFrom = uci.substr(0, 2);
        strTo = uci.substr(2, 2);
        strPromotion = uci.back();
        Square::Optional from = Square::fromName(strFrom);
        Square::Optional to = Square::fromName(strTo);
        Piece::Optional promotion = Piece::fromSymbol(strPromotion);
        if (from.has_value() && to.has_value() && promotion.has_value())
            return Move(from.value(), to.value(), promotion.value().type());
        else
            return std::nullopt;
    }
    else if (uci.length() == 4)
    {
        strFrom = uci.substr(0, 2);
        strTo = uci.substr(2, 2);
        Square::Optional from = Square::fromName(strFrom);
        Square::Optional to = Square::fromName(strTo);
        if (from.has_value() && to.has_value())
            return Move(from.value(), to.value());
        else
            return std::nullopt;
    }
    else
        return std::nullopt;
}

Square Move::from() const {
    return mvFrom;
}

Square Move::to() const {
    return mvTo;
}

std::optional<PieceType> Move::promotion() const {
    return mvPromotion;
}

int Move::score() const
{
    return moveScore;
}

void Move::setScore(int s)
{
    moveScore = s;
}

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os << move.from();
    os << move.to();
    if (move.promotion().has_value())
        os << move.promotion().value();
    return os;
}


bool operator<(const Move& lhs, const Move& rhs) {
    if (lhs.score() < rhs.score())
        return true;
    
    if (lhs.from() < rhs.from())
        return true;
    else if (lhs.from() == rhs.from())
    {
        if (lhs.to() < rhs.to())
            return true;
        else if ((lhs.to() == rhs.to()))
        {
            if (lhs.promotion() == rhs.promotion())
                return false;
            else if (rhs.promotion() == std::nullopt)
                return false;
            else if (lhs.promotion() == std::nullopt)
                return true;
            else if (lhs.promotion().value() < rhs.promotion().value())
                return true;
            else
                return false;
        }
        else
            return false;
    }
    else
        return false;

}

bool operator>(const Move& lhs, const Move& rhs) {
    return (lhs.score() > rhs.score());
}


bool operator==(const Move& lhs, const Move& rhs) {
    if (lhs.from() == rhs.from() && lhs.to() == rhs.to() && lhs.promotion() == rhs.promotion() && lhs.score() == rhs.score())
        return true;
    else
        return false;
}



