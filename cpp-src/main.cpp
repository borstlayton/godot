#include "minimax.h"
#include <iostream>

int main(){
    char board[8][8];
    char pieces[9] = "RNBQKBNR";
    char pawnsW[9] = "PPPPPPPP";
    char pawnsB[9] = "pppppppp";
    char row3[9] = "00000000";
    char row4[9] = "00000000";
    for (int i=0;i<8;i++){
        board[i][0] = pieces[i];
        board[i][1] = pawnsW[i];
        board[i][2] = '0';
        board[i][3] = row3[i];
        board[i][4] = row4[i];
        board[i][5] = '0';
        board[i][6] = pawnsB[i];
        board[i][7] = pieces[i] + 32;
    }

    // Move ret = engine.findBest(board, true, 4);
    // std::vector<Move> moves = engine.generateAttacks(board,true);
    // std::vector<Move> moves;
    // std::cout << engine.tryAddMove(board, Move(Square(1,1), Square(1,2), 0), true, moves) << std::endl;
    // std::cout << moves.size() << std::endl;
    // engine.generatePawnMove(board, Square(1,1), true, moves);
    // moves.push_back(Move(Square(1,1), Square(1,2), 0));

    // std::cout << engine.isValidMove(board, moves[0], true);
    // std::cout << engine.inCheck(board, true);
    

    
    // std::cout << "(" << char(ret.sq1.x + 97) << "," << ret.sq1.y+1 << ") -> (" << char(ret.sq2.x + 97) << "," << ret.sq2.y+1 << ")\n";

    // for (Move mv : moves){
    //     mv.printMove();
    // }
    Minimax engine; 
    Move ret = engine.findBest(board, true, 8);
    std::cout << "Boards searched:" << engine.num_boards << std::endl;
    std::cout << "Hash Matches:" << engine.hashes_found << std::endl;
    std::cout << "Best Move:"; 
    ret.printMove();
    
    // bool tf = engine.inCheck(board, false);
    // std::cout << "Check:" << tf << std::endl; 
    // engine.printBoard(board);
    // std::vector<Move> my = engine.generateMoves(board, false);
    // std::cout << "Moves:\n";
    // for(Move &mv : my){
    //     mv.printMove();
    // }

    // std::cout << "Attacks:\n";
    // std::vector<Move> att = engine.generateAttacks(board, false);
    // for(Move &mv : att){
    //     mv.printMove();
    // }

    // Move("d2d4").printMove();
        
    return 0;
}