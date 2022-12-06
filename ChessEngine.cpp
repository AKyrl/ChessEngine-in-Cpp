#include <ChessEngine.hpp>

std::string ChessEngine::name() const
{
	return "r0785071";
}

std::string ChessEngine::version() const
{
	return "v1.0";
}

std::string ChessEngine::author() const
{
	return "Alexandros Kyrloglou";
}

void ChessEngine::newGame()
{
}

PrincipalVariation ChessEngine::pv(const Board& board, const TimeInfo::Optional& timeInfo)
{
	(void)board;
	(void)timeInfo;
	return PrincipalVariation();
}
