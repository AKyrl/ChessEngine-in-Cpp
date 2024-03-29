#ifndef CHESS_ENGINE_ENGINE_HPP
#define CHESS_ENGINE_ENGINE_HPP

#include "PrincipalVariation.hpp"
#include "Board.hpp"
#include "TimeInfo.hpp"

#include <string>
#include <optional>
#include <cstddef>

struct HashInfo {
    std::size_t defaultSize;
    std::size_t minSize;
    std::size_t maxSize;
};

class Engine {
public:

    virtual ~Engine() = default;

    virtual std::string name() const = 0;
    virtual std::string version() const = 0;
    virtual std::string author() const = 0;

    virtual void newGame() = 0;
    virtual PrincipalVariation pv(
        Board& board,
        const TimeInfo::Optional& timeInfo = std::nullopt
    ) = 0;

    virtual std::optional<HashInfo> hashInfo() const;
    virtual void setHashSize(std::size_t size);
};

#endif
