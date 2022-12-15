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

PrincipalVariation ChessEngine::pv(Board& board, const TimeInfo::Optional& timeInfo)
{
	(void)timeInfo;

	PrincipalVariation pv;
	int score = abSearch(3, -std::numeric_limits<int>::infinity(), std::numeric_limits<int>::infinity(), board, &pv);
	pv.setScore(score);

	return pv;
}

int ChessEngine::abSearch(int depth, int alpha, int beta, Board& board, PrincipalVariation * pPV)
{
	PrincipalVariation pv;
	int score = -std::numeric_limits<int>::infinity();;

	if (depth < 1)
	{
		pPV->pvMoves.clear();
		return board.evaluate();
	}
	Board::MoveVec moves;
	board.pseudoLegalMoves(moves);

	for (Move m : moves)
	{
		board.storeBoard();
		board.makeMove(m);
		score = -abSearch(depth - 1, -beta, -alpha, board, &pv);
		board.restoreBoard();
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
			pPV->pvMoves.push_back(m);
			pPV->pvMoves.insert(pPV->pvMoves.end(), pv.pvMoves.begin(), pv.pvMoves.end());
		}
	}
	if (moves.empty())
	{
		if (board.isKingInCheck()) {
			pPV->setMate(true);
			return -10000 + 10 * (int)pv.length(); // mate
		}
		else
			return 0; //stalemate
	}

	return alpha;
}
