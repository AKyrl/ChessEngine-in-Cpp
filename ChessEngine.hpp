#ifndef CHESS_ENGINE_CHESSENGINE_HPP
#define CHESS_ENGINE_CHESSENGINE_HPP

#include "Engine.hpp"

#include <memory>
#include <array>
#include <chrono>


const int inf = 50000;

 // MVV LVA scores[attack][capture]
const int mvv_lva[12][12] = {
    105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

    105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
    104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
    103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
    102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
    101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
    100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};




class ChessEngine : public Engine {
    int turns = 3;
    Move killerScores[2][64];
    int historyScores[12][64];
    bool followingPV = false;
    bool scorePV = false;

    std::chrono::milliseconds maxTime = (std::chrono::milliseconds)20000;
    std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
public:

    std::string name() const;
    std::string version() const;
    std::string author() const;

    void newGame();
    PrincipalVariation pv(
        Board& board,
        const TimeInfo::Optional& timeInfo = std::nullopt
    );

    int quiescence(int alpha, int beta, Board &board, PrincipalVariation *pPV);
    int abSearch(int depth, int alpha, int beta, Board& board, PrincipalVariation * pv);

    //score move using MMV LVA
    int scoreMove(Move &move, Board &board, const PrincipalVariation &pv);
};


#endif