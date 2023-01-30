#include "Board.hpp"
#include "Board.hpp"




#include <ostream>
#include <cassert>
#include <cmath>


Board::Board()
{
    boardTurn = PieceColor::White;
    boardEnpassantSquare = std::nullopt;
    boardCastlingRights = CastlingRights::All;

    std::fill(std::begin(bitboards), std::end(bitboards), 0ULL);
    std::fill(std::begin(occupancies), std::end(occupancies), 0ULL);

    boardAttacks = Attacks();

    ////init backups
    //std::fill(std::begin(backup_bitboards), std::end(backup_bitboards), 0ULL);
    //std::fill(std::begin(backup_occupancies), std::end(backup_occupancies), 0ULL);
    //backup_boardTurn = PieceColor::White;
    //backup_boardEnpassantSquare = std::nullopt;
    //backup_boardCastlingRights = CastlingRights::All;
        
}

void Board::setPiece(const Square& square, const Piece::Optional& piece) {
    if (!piece.has_value())
        return;
   
    if (getBit(occupancies[Both], square.index())) {
        for (int i = P; i <= k; i++) {
            if (getBit(bitboards[i], square.index())) {
                popBit(bitboards[i], square.index());
                break;
            }
        }
        popBit(occupancies[Both], square.index());
        popBit(occupancies[Black], square.index());
        popBit(occupancies[White], square.index());
    }

    setBit(bitboards[getBitboardIndex(piece)], square.index());
    setBit(occupancies[getOccupanciesIndex(piece)], square.index());
    setBit(occupancies[Both], square.index());
    
}

void Board::removePiece(const Square& square, Piece piece) 
{
    popBit(occupancies[Both], square.index());
    popBit(occupancies[getOccupanciesIndex(piece)], square.index());
    popBit(bitboards[getBitboardIndex(piece)], square.index());
}

Piece::Optional Board::piece(const Square& square) const {
    if ( !getBit( occupancies[Both], square.index() ) )
        return std::nullopt;
    else
    {
        for (int i = P; i <= k; i++) {
            if (getBit(bitboards[i], square.index()))
                return getPieceFromIndex(i);
        }
    return std::nullopt;
    }
}

void Board::setTurn(PieceColor turn) {
    boardTurn = turn;
}

PieceColor Board::turn() const {
    return boardTurn;
}

void Board::setCastlingRights(CastlingRights cr) {
    boardCastlingRights = cr;
}

CastlingRights Board::castlingRights() const {
    return boardCastlingRights;
}

void Board::setEnPassantSquare(const Square::Optional& square) {
    boardEnpassantSquare = square;
}

Square::Optional Board::enPassantSquare() const {
    return boardEnpassantSquare;
}

void Board::makeMove(const Move& move) {
    
    Piece::Optional optPiece = Board::piece(move.from());
    if (!optPiece.has_value())
        return;
    Piece fromPiece = optPiece.value();
    //Piece::Optional optToPiece = Board::piece(move.to());
    

    if (move.promotion().has_value())
    {
        removePiece(move.from(), fromPiece);
        setPiece(move.to(), Piece(turn(), move.promotion().value()));
    }
    else if (fromPiece.type() == PieceType::King)
    {
        int diff = std::abs((int)move.from().file() - (int)move.to().file());
        // if casting move rook
        if (diff > 1)
        {
            if (move.to() == Square::G1)
            {
                removePiece(Square::H1, Piece(PieceColor::White, PieceType::Rook));
                setPiece(Square::F1, Piece(PieceColor::White, PieceType::Rook));
            }
            else if (move.to() == Square::G8)
            {
                removePiece(Square::H8, Piece(PieceColor::Black, PieceType::Rook));
                setPiece(Square::F8, Piece(PieceColor::Black, PieceType::Rook));
            }
            else  if (move.to() == Square::C1)
            {
                removePiece(Square::A1, Piece(PieceColor::White, PieceType::Rook));
                setPiece(Square::D1, Piece(PieceColor::White, PieceType::Rook));
            }
            else  if (move.to() == Square::C8)
            {
                removePiece(Square::A8, Piece(PieceColor::Black, PieceType::Rook));
                setPiece(Square::D8, Piece(PieceColor::Black, PieceType::Rook));
            }
        }
        //normal king move
        removePiece(move.from(), fromPiece);
        setPiece(move.to(), fromPiece);
        if (fromPiece.color() == PieceColor::White)
            setCastlingRights(boardCastlingRights & ~CastlingRights::White);
        else
            setCastlingRights(boardCastlingRights & ~CastlingRights::Black);
    }
    else
    {
        //remove casting rights if a rook is moved or captured
        if (move.from() == Square::A1 || move.to() == Square::A1)
            setCastlingRights(boardCastlingRights & ~CastlingRights::WhiteQueenside);
        if (move.from() == Square::H1 || move.to() == Square::H1)
            setCastlingRights(boardCastlingRights & ~CastlingRights::WhiteKingside);
        if (move.from() == Square::A8 || move.to() == Square::A8)
            setCastlingRights(boardCastlingRights & ~CastlingRights::BlackQueenside);
        if (move.from() == Square::H8 || move.to() == Square::H8)
            setCastlingRights(boardCastlingRights & ~CastlingRights::BlackKingside);
        
        //remove en Passant capture
        if (fromPiece.type() == PieceType::Pawn && move.to() == enPassantSquare())
        {
            if(fromPiece.color() == PieceColor::White)
                removePiece(Square::fromCoordinates(enPassantSquare().value().file(), 4).value(), Piece(PieceColor::Black, PieceType::Pawn));
            else
                removePiece(Square::fromCoordinates(enPassantSquare().value().file(), 3).value(), Piece(PieceColor::White, PieceType::Pawn));
        }

        // set en Passant square
        if (fromPiece.type() == PieceType::Pawn && std::abs((int)move.from().rank() - (int)move.to().rank()) > 1)
        {
            if (fromPiece.color() == PieceColor::White)
                setEnPassantSquare(Square::fromCoordinates(move.from().file(), 2));
            else
                setEnPassantSquare(Square::fromCoordinates(move.from().file(), 5));
        }
        else
            setEnPassantSquare(std::nullopt);

        removePiece(move.from(), fromPiece);
        setPiece(move.to(), fromPiece);
    }


    setTurn(!turn());
    }

void Board::pseudoLegalMoves(MoveVec& moves){
    
    uint64_t bitboard[6];
    uint64_t attacks;
    int pawnMove, turn;
    Square::Coordinate promotionRank, startRank;
    

    if (boardTurn == PieceColor::White)
    {
        bitboard[0] = bitboards[P];
        bitboard[1] = bitboards[N];
        bitboard[2] = bitboards[B];
        bitboard[3] = bitboards[R];
        bitboard[4] = bitboards[Q];
        bitboard[5] = bitboards[K];
        pawnMove = +8;
        promotionRank = 7;
        startRank = 1;
        turn = White;
    }
    else
    {
        bitboard[0] = bitboards[p];
        bitboard[1] = bitboards[n];
        bitboard[2] = bitboards[b];
        bitboard[3] = bitboards[r];
        bitboard[4] = bitboards[q];
        bitboard[5] = bitboards[k];
        pawnMove = -8;
        promotionRank = 0;
        startRank = 6;
        turn = Black;
    }

    // get Pawn moves
    while (bitboard[P])
    {
        Square fromSquare = Square::fromIndex(getLSPawnIndex(bitboard[P])).value();

        Square::Optional opToSquare = Square::fromIndex(fromSquare.index() + pawnMove);
        if (opToSquare.has_value())
        {
            // plain moves
            if (!getBit(occupancies[Both], opToSquare.value().index()))
            {
                if (opToSquare.value().rank() == promotionRank)
                {
                    addMoveIfLegal(moves, Move(fromSquare, opToSquare.value(), PieceType::Knight));
                    addMoveIfLegal(moves, Move(fromSquare, opToSquare.value(), PieceType::Bishop));
                    addMoveIfLegal(moves, Move(fromSquare, opToSquare.value(), PieceType::Rook));
                    addMoveIfLegal(moves, Move(fromSquare, opToSquare.value(), PieceType::Queen));
                }
                else
                {
                    addMoveIfLegal(moves, Move(fromSquare, opToSquare.value()));

                    opToSquare = Square::fromIndex(fromSquare.index() + pawnMove + pawnMove);
                    if (opToSquare.has_value() && fromSquare.rank() == startRank)
                    {
                        if (!getBit(occupancies[Both], opToSquare.value().index()))
                            addMoveIfLegal(moves, Move(fromSquare, opToSquare.value()));
                    }
                }
            }
        }

        if(boardTurn == PieceColor::White)
            attacks = boardAttacks.pawnAttacks[turn][fromSquare.index()] & occupancies[Black];
        else
            attacks = boardAttacks.pawnAttacks[turn][fromSquare.index()] & occupancies[White];

        // pawn captures
        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();

            if (toSquare.rank() == promotionRank)
            {
                addMoveIfLegal(moves, Move(fromSquare, toSquare, PieceType::Knight));
                addMoveIfLegal(moves, Move(fromSquare, toSquare, PieceType::Bishop));
                addMoveIfLegal(moves, Move(fromSquare, toSquare, PieceType::Rook));
                addMoveIfLegal(moves, Move(fromSquare, toSquare, PieceType::Queen));
            }
            else
                addMoveIfLegal(moves, (Move(fromSquare, toSquare)));

            popBit(attacks, toSquare.index());
        }

        // get En Passant moves
        if (boardEnpassantSquare.has_value())
        {
            if (boardAttacks.pawnAttacks[turn][fromSquare.index()] & (1ULL << boardEnpassantSquare.value().index()) )
            {
                addMoveIfLegal(moves, Move(fromSquare, boardEnpassantSquare.value()));
            }
        }

        // pop ls1b from piece bitboard copy
        popBit(bitboard[P], fromSquare.index());
    }

    // get Knigt moves
    while (bitboard[N])
    {
        Square fromSquare = Square::fromIndex(getLSPawnIndex(bitboard[N])).value();
        attacks = boardAttacks.knightAttacks[fromSquare.index()] & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(fromSquare, toSquare));

            popBit(attacks, toSquare.index());
        }

        popBit(bitboard[N], fromSquare.index());
    }

    // get Bishop moves
    while (bitboard[B])
    {
        Square fromSquare = Square::fromIndex(getLSPawnIndex(bitboard[B])).value();
        attacks = boardAttacks.getBishopAttacks(fromSquare.index(), occupancies[Both]) & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(fromSquare, toSquare));

            popBit(attacks, toSquare.index());
        }

        popBit(bitboard[B], fromSquare.index());
    }

    // get Rook moves
    while (bitboard[R])
    {
        Square fromSquare = Square::fromIndex(getLSPawnIndex(bitboard[R])).value();
        attacks = boardAttacks.getRookAttacks(fromSquare.index(), occupancies[Both]) & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(fromSquare, toSquare));

            popBit(attacks, toSquare.index());
        }

        popBit(bitboard[R], fromSquare.index());
    }

    // get Queen moves
    while (bitboard[Q])
    {
        Square fromSquare = Square::fromIndex(getLSPawnIndex(bitboard[Q])).value();
        attacks = boardAttacks.getQueenAttacks(fromSquare.index(), occupancies[Both]) & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(fromSquare, toSquare));

            popBit(attacks, toSquare.index());
        }

        popBit(bitboard[Q], fromSquare.index());
    }

    // get King moves
    while (bitboard[K])
    {
        Square fromSquare = Square::fromIndex(getLSPawnIndex(bitboard[K])).value();
        attacks = boardAttacks.kingAttacks[fromSquare.index()] & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(fromSquare, toSquare));

            popBit(attacks, toSquare.index());
        }

        //Casting moves
        if (boardTurn == PieceColor::White)
        {
            if ((boardCastlingRights & CastlingRights::WhiteKingside) == CastlingRights::WhiteKingside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 5) && !getBit(occupancies[Both], 6))
                {
                    // king possitions are not attacked
                    if (!isSquareAttacked(4, PieceColor::Black) && !isSquareAttacked(5, PieceColor::Black) && !isSquareAttacked(6, PieceColor::Black))
                        addMoveIfLegal(moves, Move(fromSquare, Square::fromIndex(6).value()));
                }
            }
            else if( (boardCastlingRights & CastlingRights::WhiteQueenside) == CastlingRights::WhiteQueenside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 1) && !getBit(occupancies[Both], 2) && !getBit(occupancies[Both], 3))
                {
                    // king possitions are not attacked from to and there
                    if (!isSquareAttacked(2, PieceColor::Black) && !isSquareAttacked(3, PieceColor::Black) && !isSquareAttacked(4, PieceColor::Black))
                        addMoveIfLegal(moves, Move(fromSquare, Square::fromIndex(2).value()));
                }
            }
        }
        else
        {
            if ((boardCastlingRights & CastlingRights::BlackKingside) == CastlingRights::BlackKingside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 62) && !getBit(occupancies[Both], 61))
                {
                    // king possitions are not attacked
                    if (!isSquareAttacked(60, PieceColor::Black) && !isSquareAttacked(61, PieceColor::Black) && !isSquareAttacked(62, PieceColor::Black))
                        addMoveIfLegal(moves, Move(fromSquare, Square::fromIndex(62).value()));
                }
            }
            else if ((boardCastlingRights & CastlingRights::BlackQueenside) == CastlingRights::BlackQueenside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 59) && !getBit(occupancies[Both], 58) && !getBit(occupancies[Both], 57))
                {
                    // king possitions are not attacked from to and there
                    if (!isSquareAttacked(60, PieceColor::Black) && !isSquareAttacked(59, PieceColor::Black) && !isSquareAttacked(58, PieceColor::Black))
                        addMoveIfLegal(moves, Move(fromSquare, Square::fromIndex(58).value()));
                }
            }
        }

        popBit(bitboard[K], fromSquare.index());
    }

}

void Board::pseudoLegalMovesFrom(const Square& from,
                                 Board::MoveVec& moves) {
    uint64_t attacks;

    Piece::Optional piece = Board::piece(from);
    if (!piece.has_value())
        return;

    switch (piece.value().type())
    {
    case PieceType::Pawn:
    {
        int pawnMove, turn;
        Square::Coordinate promotionRank, startRank;
        if (boardTurn == PieceColor::White)
        {
            pawnMove = +8;
            promotionRank = 7;
            startRank = 1;
            turn = White;
            attacks = boardAttacks.pawnAttacks[turn][from.index()] & occupancies[Black];
        }
        else
        {
            pawnMove = -8;
            promotionRank = 0;
            startRank = 6;
            turn = Black;
            attacks = boardAttacks.pawnAttacks[turn][from.index()] & occupancies[White];
        }

        Square::Optional opToSquare = Square::fromIndex(from.index() + pawnMove);
        if (opToSquare.has_value())
        {
            // plain moves
            if (!getBit(occupancies[Both], opToSquare.value().index()))
            {
                if (opToSquare.value().rank() == promotionRank)
                {
   
                    addMoveIfLegal(moves, Move(from, opToSquare.value(), PieceType::Knight));
                    addMoveIfLegal(moves, Move(from, opToSquare.value(), PieceType::Bishop));
                    addMoveIfLegal(moves, Move(from, opToSquare.value(), PieceType::Rook));
                    addMoveIfLegal(moves, Move(from, opToSquare.value(), PieceType::Queen));
                }
                else
                {
                    addMoveIfLegal(moves, Move(from, opToSquare.value()));

                    opToSquare = Square::fromIndex(from.index() + pawnMove + pawnMove);
                    if (opToSquare.has_value() && from.rank() == startRank)
                    {
                        if (!getBit(occupancies[Both], opToSquare.value().index()))
                            addMoveIfLegal(moves, Move(from, opToSquare.value()));
                    }
                }
            }
        }

        // pawn captures
        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();

            if (toSquare.rank() == promotionRank)
            {
                addMoveIfLegal(moves, Move(from, toSquare, PieceType::Knight));
                addMoveIfLegal(moves, Move(from, toSquare, PieceType::Bishop));
                addMoveIfLegal(moves, Move(from, toSquare, PieceType::Rook));
                addMoveIfLegal(moves, Move(from, toSquare, PieceType::Queen));
            }
            else
                addMoveIfLegal(moves, Move(from, toSquare));

            popBit(attacks, toSquare.index());
        }

        // get En Passant moves
        if (boardEnpassantSquare.has_value())
        {
            if (boardAttacks.pawnAttacks[turn][from.index()] & (1ULL << boardEnpassantSquare.value().index()))
                addMoveIfLegal(moves, Move(from, boardEnpassantSquare.value()));
        }

        break;
    }
    case PieceType::Knight:
    {
        attacks = boardAttacks.knightAttacks[from.index()] & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(from, toSquare));

            popBit(attacks, toSquare.index());
        }
        break;
    }
    case PieceType::Bishop:
    {
        attacks = boardAttacks.getBishopAttacks(from.index(), occupancies[Both]) & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(from, toSquare));

            popBit(attacks, toSquare.index());
        }
        break;
    }
    case PieceType::Rook:
    {
        attacks = boardAttacks.getRookAttacks(from.index(), occupancies[Both]) & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(from, toSquare));

            popBit(attacks, toSquare.index());
        }
        break;
    }
    case PieceType::Queen:
    {
        attacks = boardAttacks.getQueenAttacks(from.index(), occupancies[Both]) & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);

        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(from, toSquare));

            popBit(attacks, toSquare.index());
        }
        break;
    }
    case PieceType::King:
    {
        attacks = boardAttacks.kingAttacks[from.index()] & ((boardTurn == PieceColor::White) ? ~occupancies[White] : ~occupancies[Black]);
        while (attacks)
        {
            Square toSquare = Square::fromIndex(getLSPawnIndex(attacks)).value();
            addMoveIfLegal(moves, Move(from, toSquare));

            popBit(attacks, toSquare.index());
        }

        //Casting moves
        if (boardTurn == PieceColor::White)
        {
            if ((boardCastlingRights & CastlingRights::WhiteKingside) == CastlingRights::WhiteKingside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 5) && !getBit(occupancies[Both], 6))
                {
                    // king possitions are not attacked
                    if (!isSquareAttacked(4, PieceColor::Black) && !isSquareAttacked(5, PieceColor::Black) && !isSquareAttacked(6, PieceColor::Black))
                        addMoveIfLegal(moves, Move(from, Square::fromIndex(6).value()));
                }
            }
            else if ((boardCastlingRights & CastlingRights::WhiteQueenside) == CastlingRights::WhiteQueenside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 1) && !getBit(occupancies[Both], 2) && !getBit(occupancies[Both], 3))
                {
                    // king possitions are not attacked from to and there
                    if (!isSquareAttacked(2, PieceColor::Black) && !isSquareAttacked(3, PieceColor::Black) && !isSquareAttacked(4, PieceColor::Black))
                        addMoveIfLegal(moves, Move(from, Square::fromIndex(2).value()));
                }
            }
        }
        else
        {
            if ((boardCastlingRights & CastlingRights::BlackKingside) == CastlingRights::BlackKingside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 62) && !getBit(occupancies[Both], 61))
                {
                    // king possitions are not attacked
                    if (!isSquareAttacked(60, PieceColor::White) && !isSquareAttacked(61, PieceColor::White) && !isSquareAttacked(62, PieceColor::White))
                        addMoveIfLegal(moves, Move(from, Square::fromIndex(62).value()));
                }
            }
            else if ((boardCastlingRights & CastlingRights::BlackQueenside) == CastlingRights::BlackQueenside)
            {
                //check if middle possitions are empty
                if (!getBit(occupancies[Both], 59) && !getBit(occupancies[Both], 58) && !getBit(occupancies[Both], 57))
                {
                    // king possitions are not attacked from to and there
                    if (!isSquareAttacked(60, PieceColor::White) && !isSquareAttacked(59, PieceColor::White) && !isSquareAttacked(58, PieceColor::White))
                        addMoveIfLegal(moves, Move(from, Square::fromIndex(58).value()));
                }
            }
        }

        break;
    }
    default:
        break;
    } 
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
    os << "Board state: ";
    os << board.castlingRights();
    os << " ";
    if (board.enPassantSquare().has_value()) {
        os << board.enPassantSquare().value();
        os << " ";
    }
    else
        os << "- ";
    os << board.turn();
    os << "\n";
    for (int i = 0; i < 64; i++) {
        Square::Optional square =  Square::fromIndex(i);
        if (square.has_value())
        {
            
            Piece::Optional piece = board.piece(square.value());
            if (piece.has_value()) {
                os << piece.value();
            }
            else
            {
                os << ".";
            }
            os << " ";
            if ( (i+1) % 8 == 0)
                os << "\n";
        }
    }
    return os;
}



bool Board::isSquareAttacked(int square, PieceColor side) const
{
    if (square < 0 || square >= 64)
        return false;
    if (side == PieceColor::White)
    {
        if (boardAttacks.pawnAttacks[Black][square] & bitboards[P])
            return true;
        if (boardAttacks.knightAttacks[square] & bitboards[N])
            return true;
        if (boardAttacks.getBishopAttacks(square, occupancies[Both]) & bitboards[B])
            return true;
        if (boardAttacks.getRookAttacks(square, occupancies[Both]) & bitboards[R])
            return true;
        if (boardAttacks.getQueenAttacks(square, occupancies[Both]) & bitboards[Q])
            return true;
        if (boardAttacks.kingAttacks[square] & bitboards[K])
            return true;
    }
    else
    {
        if (boardAttacks.pawnAttacks[White][square] & bitboards[p])
            return true;
        if (boardAttacks.knightAttacks[square] & bitboards[n])
            return true;
        if (boardAttacks.getBishopAttacks(square, occupancies[Both]) & bitboards[b])
            return true;
        if (boardAttacks.getRookAttacks(square, occupancies[Both]) & bitboards[r])
            return true;
        if (boardAttacks.getQueenAttacks(square, occupancies[Both]) & bitboards[q])
            return true;
        if (boardAttacks.kingAttacks[square] & bitboards[k])
            return true;
    }

    return false;
}

bool Board::isKingInCheck()
{
    uint64_t bb = (turn() == PieceColor::White) ? bitboards[K] : bitboards[k];
    return isSquareAttacked(getLSPawnIndex(bb), !turn());
}


void Board::addMoveIfLegal(Board::MoveVec& moves, Move move) 
{
    Piece::Optional optPiece = Board::piece(move.from());
    if (!optPiece.has_value())
        return;
    Piece fromPiece = optPiece.value();

    BoardCoppy copy = storeBoard();
    
    if (fromPiece.type() == PieceType::King)
    {
        int diff = std::abs((int)move.from().file() - (int)move.to().file());
        // if casting move rook
        if (diff > 1)
        {
            if (move.to() == Square::G1)
            {
                removePiece(Square::H1, Piece(PieceColor::White, PieceType::Rook));
                setPiece(Square::F1, Piece(PieceColor::White, PieceType::Rook));
            }
            else if (move.to() == Square::G8)
            {
                removePiece(Square::H8, Piece(PieceColor::Black, PieceType::Rook));
                setPiece(Square::F8, Piece(PieceColor::Black, PieceType::Rook));
            }
            else  if (move.to() == Square::C1)
            {
                removePiece(Square::A1, Piece(PieceColor::White, PieceType::Rook));
                setPiece(Square::D1, Piece(PieceColor::White, PieceType::Rook));
            }
            else  if (move.to() == Square::C8)
            {
                removePiece(Square::A8, Piece(PieceColor::Black, PieceType::Rook));
                setPiece(Square::D8, Piece(PieceColor::Black, PieceType::Rook));
            }
        }
        //normal king move
        removePiece(move.from(), fromPiece);
        setPiece(move.to(), fromPiece);
    }
    else
    {
        //remove en Passant capture
        if (fromPiece.type() == PieceType::Pawn && move.to() == enPassantSquare())
        {
            if (fromPiece.color() == PieceColor::White)
                removePiece(Square::fromCoordinates(enPassantSquare().value().file(), 4).value(), Piece(PieceColor::Black, PieceType::Pawn));
            else
                removePiece(Square::fromCoordinates(enPassantSquare().value().file(), 3).value(), Piece(PieceColor::White, PieceType::Pawn));
        }

        removePiece(move.from(), fromPiece);
        setPiece(move.to(), fromPiece);
    }

    uint64_t bb = fromPiece.color() == PieceColor::White ? bitboards[K] : bitboards[k];
    // if king is not attacked add move 
    if (!isSquareAttacked(getLSPawnIndex(bb), !fromPiece.color()))
        moves.push_back(move);

    restoreBoard(copy);

}

int Board::getGamePhase()
{
    int score = 0;

    score += countPiecies(bitboards[N]) * earlygameMaterialScore[N];
    score += countPiecies(bitboards[B]) * earlygameMaterialScore[B];
    score += countPiecies(bitboards[R]) * earlygameMaterialScore[R];
    score += countPiecies(bitboards[Q]) * earlygameMaterialScore[Q];

    score -= countPiecies(bitboards[n]) * earlygameMaterialScore[n];
    score -= countPiecies(bitboards[b]) * earlygameMaterialScore[b];
    score -= countPiecies(bitboards[r]) * earlygameMaterialScore[r];
    score -= countPiecies(bitboards[q]) * earlygameMaterialScore[q];

    if (score > earlygameScore)
        return 0; // we are in early game
    else if (score < endgameScore)
        return -1; // we are in end game
    else
        return score; // we are in mid game use this to interpolate
}

int Board::evaluate()
{
    int earlyScore = 0, endScore  = 0;
    uint64_t bb;

    for (int piece = P; piece <= k; piece++)
    {
        bb = bitboards[piece];

        while (bb)
        {
            int square = getLSPawnIndex(bb);
            
            // Add material score
            earlyScore += earlygameMaterialScore[piece];
            endScore += endgameMaterialScore[piece];

            // Add possitional score
            switch (piece)
            {
            case P:
            {
                earlyScore += earlygamePositionalScore[P][verticalflipIndex[square]];
                endScore += endgamePositionalScore[P][verticalflipIndex[square]];
                break;
            }
            case N:
            {
                earlyScore += earlygamePositionalScore[N][verticalflipIndex[square]];
                endScore += endgamePositionalScore[N][verticalflipIndex[square]];
                break;
            }
            case B:
            {
                earlyScore += earlygamePositionalScore[B][verticalflipIndex[square]];
                endScore += endgamePositionalScore[B][verticalflipIndex[square]];
                break;
            }
            case R:
            {
                earlyScore += earlygamePositionalScore[R][verticalflipIndex[square]];
                endScore += endgamePositionalScore[R][verticalflipIndex[square]];
                break;
            }
            case Q:
            {
                earlyScore += earlygamePositionalScore[Q][verticalflipIndex[square]];
                endScore += endgamePositionalScore[Q][verticalflipIndex[square]];
                break;
            }
            case K:
            {
                earlyScore += earlygamePositionalScore[K][verticalflipIndex[square]];
                endScore += endgamePositionalScore[K][verticalflipIndex[square]];
                break;
            }
            case p:
            {
                earlyScore -= earlygamePositionalScore[P][square];
                endScore -= endgamePositionalScore[P][square];
                break;
            }
            case n:
            {
                earlyScore -= earlygamePositionalScore[N][square];
                endScore -= endgamePositionalScore[N][square];
                break;
            }
            case b:
            {
                earlyScore -= earlygamePositionalScore[B][square];
                endScore -= endgamePositionalScore[B][square];
                break;
            }
            case r:
            {
                earlyScore -= earlygamePositionalScore[R][square];
                endScore -= endgamePositionalScore[R][square];
                break;
            }
            case q:
            {
                earlyScore -= earlygamePositionalScore[Q][square];
                endScore -= endgamePositionalScore[Q][square];
                break;
            }
            case k:
            {
                earlyScore -= earlygamePositionalScore[K][square];
                endScore -= endgamePositionalScore[K][square];
                break;
            }
            default:
                break;
            }

            popBit(bb, square);
        }
    }

    int phase = getGamePhase();
    int score = 0;

    if (phase == 0)
        score = earlyScore;
    else if (phase == -1)
        score = endScore;
    else
    {
        score = (earlyScore*phase + endScore*(earlygameScore - phase) ) / earlygameScore;
    }
     

    return turn() == PieceColor::White ? score : -score;
}

BoardCoppy Board::storeBoard()
{
    BoardCoppy cp;

    for (int i = 0; i < 12; i++)
        cp.backup_bitboards[i] = bitboards[i];
    for (int i = 0; i < 3; i++)
        cp.backup_occupancies[i] = occupancies[i];
    cp.backup_boardTurn = boardTurn;
    cp.backup_boardCastlingRights = boardCastlingRights;
    cp.backup_boardEnpassantSquare = boardEnpassantSquare;

    return cp;
}

void Board::restoreBoard(BoardCoppy cp)
{
    for (int i = 0; i < 12; i++)
        bitboards[i] = cp.backup_bitboards[i];
    for (int i = 0; i < 3; i++)
        occupancies[i] = cp.backup_occupancies[i];
    boardTurn = cp.backup_boardTurn;
    boardCastlingRights = cp.backup_boardCastlingRights;
    boardEnpassantSquare = cp.backup_boardEnpassantSquare;
}

bool Board::isStarting()
{
    if(occupancies[2] == 18446462598732906495ULL) //starting occ
        return true;
    // black nothing moved
    if (occupancies[1] == 18446462598732840960ULL)
        return true;
    return false;
}
