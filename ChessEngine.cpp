#include <ChessEngine.hpp>


#include <algorithm>
#include <iostream>

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
	//(void)timeInfo;

	std::fill(std::begin(killerScores[0]), std::end(killerScores[0]), Move());
	std::fill(std::begin(killerScores[1]), std::end(killerScores[1]), Move());
	for (int i = 0; i < 12; i++) {
		std::fill(std::begin(historyScores[i]), std::end(historyScores[i]), 0);
	}
	PrincipalVariation pv;

	followingPV = false;
	scorePV = false;

	int maxDepth;

	if (timeInfo.has_value())
	{
		//PrincipalVariation bestPV;
		maxDepth = 5;

		// Itarative Deepening
		for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++)
		{
			auto timeBefore = board.turn() == PieceColor::White ? timeInfo.value().white.timeLeft : timeInfo.value().black.timeLeft;

			followingPV = true;
			int score = abSearch(currentDepth, -inf, inf, board, &pv);
			pv.setScore(score);
			auto timeAfter = board.turn() == PieceColor::White ? timeInfo.value().white.timeLeft : timeInfo.value().black.timeLeft;

			if (timeAfter < timeBefore - timeAfter)
				break;
		}
	}
	else
	{
		maxDepth = 5;
		followingPV = true;

		for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++)
		{
			int score = abSearch(maxDepth, -inf, inf, board, &pv);
			pv.setScore(score);
		}
	}
	return pv;
}

int ChessEngine::quiescence(int alpha, int beta, Board &board, PrincipalVariation *pPV)
{
    PrincipalVariation pv;
	if(board.isKingInCheck())
		return abSearch(1, alpha, beta, board, pPV);
		
	int ev = board.evaluate();
	if(ev >= beta)
		return beta;
	if(ev > alpha)
		alpha = ev;

	Board::MoveVec moves;
	board.pseudoLegalMoves(moves); 
	for (Move &m : moves)
	{
		m.setScore( scoreMove(m, board, *pPV) );
	}

	std::sort(moves.begin(), moves.end(),  std::greater<Move>());

	for (Move &m : moves)
	{
		if(!board.piece(m.to()).has_value()) // only check caputre moves
			continue;

		BoardCoppy copy = board.storeBoard();
		board.makeMove(m);
		int score = -quiescence(-beta, -alpha, board, &pv);
		board.restoreBoard(copy);
		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
			pPV->pvMoves.clear();
			pPV->pvMoves.push_back(m);
			pPV->pvMoves.insert(pPV->pvMoves.end(), pv.pvMoves.begin(), pv.pvMoves.end());
		}
	}
	
	return alpha;
}

int ChessEngine::abSearch(int depth, int alpha, int beta, Board &board, PrincipalVariation *pPV)
{
	PrincipalVariation pv;
	int score = -inf;

	// hard cutoff in case of max ply reached
	if (pPV->length() > 64)
		return board.evaluate();;

	if (depth < 1)
		return quiescence(alpha, beta, board, pPV);

	Board::MoveVec moves;
	board.pseudoLegalMoves(moves);

	if (followingPV)
	{
		followingPV = false;
		for (Move& m : moves)
		{
			if (pPV->pvMoves.empty())
				break;
			if (pPV->pvMoves.front() == m)
			{
				scorePV = true;
				followingPV = true;
			}
		}
	}

	for (Move &m : moves)
	{
		m.setScore( scoreMove(m, board, *pPV));
	}

	std::sort(std::begin(moves), std::end(moves), std::greater<Move>());

	for (const Move &m : moves)
	{
		BoardCoppy copy = board.storeBoard();
		board.makeMove(m);
		score = -abSearch(depth - 1, -beta, -alpha, board, &pv);
		board.restoreBoard(copy);
		if (score >= beta)
		{
			killerScores[1][pPV->length()] = killerScores[0][pPV->length()];
			killerScores[0][pPV->length()] = m;
			return beta;
		}
		if (score > alpha)
		{
			auto optAttackPiece = board.piece(m.from());
			if(optAttackPiece.has_value()) 
				historyScores[optAttackPiece.value().toInt()][m.to().index()] += depth; 
				
			alpha = score;
			pPV->pvMoves.clear();
			pPV->pvMoves.push_back(m);
			pPV->pvMoves.insert(pPV->pvMoves.end(), pv.pvMoves.begin(), pv.pvMoves.end());
		}
	}
	if (moves.empty())
	{
		if (board.isKingInCheck()) {
			pPV->setMate(true);
			return -10000 + (int)pPV->length(); // mate
		}
		else
			return 0; //stalemate
	}

	return alpha;
}

int ChessEngine::scoreMove(Move &move,  Board &board, const PrincipalVariation &pv)
{
	
	auto optCapturePiece = board.piece(move.to());
	auto optAttackPiece = board.piece(move.from());

	if (scorePV)
	{
		if (move == pv.pvMoves.front())
		{
			scorePV = false;
			return 200000;
		}
	}

	if(!optAttackPiece.has_value())
		return 0;
    if(optCapturePiece.has_value())// it is a capture move
	{
		int capturePiece = optCapturePiece.value().toInt();
		int attackPiece = optAttackPiece.value().toInt();

		return  mvv_lva[attackPiece][capturePiece] + 100000;
	}
	else 
	{
		if(killerScores[0][pv.length()] == move)
			return 90000;
		else if(killerScores[1][pv.length()] == move)
			return 80000;
		else
			return historyScores[optAttackPiece.value().toInt()][move.to().index()];

	}

}

