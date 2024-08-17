#include "minimax.h"
#include <iostream>

int main(){
    char board[8][8];
    char pieces[9] = "RNBQKBNR";
    char pawns[9] = "PPPPPPPP";
    for (int i=0;i<8;i++){
        board[i][0] = pieces[i];
        board[i][3] = pawns[i];
        board[i][2] = '0';
        board[i][1] = '0';
        board[i][6] = '0';
        board[i][5] = '0';
        board[i][4] = pawns[i] + 32;
        board[i][7] = pieces[i] + 32;
    }
    Minimax engine; 

    // Move ret = engine.findBest(board, true, 4);
    // std::vector<Move> moves = engine.generateAttacks(board,true);
    // std::vector<Move> moves;
    // std::cout << engine.tryAddMove(board, Move(Square(1,1), Square(1,2), 0), true, moves) << std::endl;
    // std::cout << moves.size() << std::endl;
    // engine.generatePawnMove(board, Square(1,1), true, moves);
    // moves.push_back(Move(Square(1,1), Square(1,2), 0));

    // std::cout << engine.isValidMove(board, moves[0], true);
    // std::cout << engine.inCheck(board, true);

    Move ret = engine.findBest(board, true, 4);
    // std::cout << "(" << char(ret.sq1.x + 97) << "," << ret.sq1.y+1 << ") -> (" << char(ret.sq2.x + 97) << "," << ret.sq2.y+1 << ")\n";

    for (Move mv : moves){
        mv.printMove();
    }
        
    return 0;
}