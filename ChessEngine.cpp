#include <ChessEngine.hpp>


#include <algorithm>
//#include <iostream>


std::string ChessEngine::name() const
{
	return "ChessKnave9000";
}

std::string ChessEngine::version() const
{
	return "1.1";
}

std::string ChessEngine::author() const
{
	return "Alexandros Kyrloglou";
}

void ChessEngine::newGame()
{
	turns = 3;
}

PrincipalVariation ChessEngine::pv(Board& board, const TimeInfo::Optional& timeInfo)
{
	if (board.isStarting())
		turns = 0;
	if (turns == 0)
	{
		PrincipalVariation pv;
		if (board.turn() == PieceColor::White)
			pv.pvMoves.push_back(Move(Square::G1, Square::F3));
		else
			pv.pvMoves.push_back(Move(Square::G8, Square::F6));
		turns++;
		return pv;
	}
	if (turns == 1)
	{
		PrincipalVariation pv;
		if (board.turn() == PieceColor::White)
			pv.pvMoves.push_back(Move(Square::D2, Square::D4));
		else {
			if(board.piece(Square::D4).has_value())
				pv.pvMoves.push_back(Move(Square::D7, Square::D5));
			else
				pv.pvMoves.push_back(Move(Square::E7, Square::E5));
		}
		turns++;
		return pv;
	}
	turns++;

	std::fill(std::begin(killerScores[0]), std::end(killerScores[0]), Move());
	std::fill(std::begin(killerScores[1]), std::end(killerScores[1]), Move());
	for (int i = P; i <= k; i++) {
		std::fill(std::begin(historyScores[i]), std::end(historyScores[i]), 0);
	}
	PrincipalVariation pv;

	followingPV = false;
	scorePV = false;

	int alpha = -inf;
	int beta = inf;
	int maxDepth;

	if (timeInfo.has_value())
	{
		auto sideTimeInfo = board.turn() == PieceColor::White ? timeInfo.value().white : timeInfo.value().black;

		if (timeInfo.value().movesToGo.has_value())
			maxTime = sideTimeInfo.timeLeft / (timeInfo.value().movesToGo.value() + 1) - (std::chrono::milliseconds)200;
		else
			maxTime = sideTimeInfo.timeLeft / std::max(1, 45 - turns) - (std::chrono::milliseconds)200;
		
		maxDepth = 9;
		start = std::chrono::system_clock::now();
		// Itarative Deepening
		for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++)
		{
			
			followingPV = true;
			int score = abSearch(currentDepth, alpha, beta, board, &pv);
			
			//std::cout << "Current depth = " << currentDepth << std::endl;
			auto time = std::chrono::system_clock::now() - start;
			if (time >= maxTime)
				break;

			if ((score <= alpha) || (score >= beta)) {
				alpha = -inf;
				beta = inf;
				currentDepth--; //go again at same depth 
				continue;
			}
			alpha = score - 50;
			beta = score + 50;
		}
	}
	else
	{
		maxTime = (std::chrono::milliseconds)20000;
		maxDepth = 5;
		followingPV = true;
		start = std::chrono::system_clock::now();
		for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++)
		{
			int score = abSearch(maxDepth, alpha, beta, board, &pv);

			auto time = std::chrono::system_clock::now() - start;
			if (time >= maxTime)
				break;

			if ((score <= alpha) || (score >= beta)) {
				alpha = -inf;    
				beta = inf;      
				currentDepth--; //go again at same depth 
				continue;
			}
			alpha = score - 50;  
			beta = score + 50;
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

		auto time = std::chrono::system_clock::now() - start;
		if (time >= maxTime)
			return 0;

		if (score >= beta)
			return beta;
		if (score > alpha)
		{
			alpha = score;
			pPV->setScore(score);
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
		return board.evaluate();

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

		auto time = std::chrono::system_clock::now() - start;
		if (time >= maxTime)
			return 0;

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
			pPV->setScore(score);
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
	if (optAttackPiece.value().type() == PieceType::Pawn && move.to() == board.enPassantSquare())// enpassat capture
	{
		auto pawnColor = optAttackPiece.value().color();
		if(pawnColor == PieceColor::White)
			return  mvv_lva[P][p] + 100000;
		else
			return  mvv_lva[p][P] + 100000;
	}
	//rank Castiling higher in search
	if (optAttackPiece.value().type() == PieceType::King && std::abs((int)move.from().file() - (int)move.to().file()) > 1)
	{
		return 70000;
	}
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

