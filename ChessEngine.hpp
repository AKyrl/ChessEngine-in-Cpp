#ifndef CHESS_ENGINE_CHESSENGINE_HPP
#define CHESS_ENGINE_CHESSENGINE_HPP

#include "Engine.hpp"

#include <memory>

class ChessEngine : public Engine {
public:

    std::string name() const;
    std::string version() const;
    std::string author() const;

    void newGame();
    PrincipalVariation pv(
        const Board& board,
        const TimeInfo::Optional& timeInfo = std::nullopt
    );

};

#endif