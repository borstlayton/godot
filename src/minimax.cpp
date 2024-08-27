#include "minimax.h"
#include <godot_cpp/classes/vector4.hpp>
#include <iostream>
#include <cmath>
#include <ctime>

using namespace godot;

void Minimax::_bind_methods() {
    ClassDB::bind_method(D_METHOD("findBest"), &Minimax::findBest);
	ClassDB::bind_method(D_METHOD("setDepth", "depth"), &Minimax::setDepth);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "depth", PROPERTY_HINT_RANGE, "6,12,1"), "setDepth");
}

void Minimax::_ready(){
	Ref GDboardScript = ResourceLoader::load(“board.gd”, “Script”);
	Node2D *GDboard = ClassDB::instance(GDboardScript->get_instance_base_type());
	GDboard->set_script(GDboardScript.get_ref_ptr());
	setDepth(8);
}

// Returns the best move
vector4 Minimax::findBest(){
	Chess Board(GDboard->call("get_board"));

	//Generate all moves, with attacks first
	std::vector<Move> all_moves = generateAll();

	float best_val = (Board->turn) ? -INFINITY : INFINITY;
	Move best_move;

	for (Move& mv : all_moves){
		Board->num_boards++;

		//Make the move
		char target = Board->at(mv.sq2); //Holds what is on the target square
		Board->set(mv.sq2, mv.promotion ? mv.promotion : Board->at(mv.sq1)); //If a promotion, set target square to the promoted piece
		Board->set(mv.sq1,'0'); //Set the start square to blank

		Board->turn = !Board->turn;
		float val = abMinimax(Board, depth-1, -INFINITY, INFINITY);
		Board->turn = !Board->turn;

		//Undo the move
		Board->set(mv.sq1, mv.promotion ? (Board->turn ? 'P' : 'p') : Board->at(mv.sq2)); //If a promotion, set target square to the promoted piece
		Board->set(mv.sq2, target);

		//Update best_val and best_move
		if ((Board->turn && val > best_val) || (!Board->turn && val < best_val)){
			best_val = val;
			best_move = mv;
		}
	}
	// return best_move;
	return vector4(mv.sq1.x,mv.sq1.y,mv.sq2.x,mv.sq2.y);
}

//minimax with ab pruning, and a fixed depth
float Minimax::abMinimax(Chess &Board, unsigned int depth, float alpha, float beta){
	if (depth == 0){
		// return eval(board);
		return Board->abMinimaxAttacks(Board, alpha, beta);
	}else{
		//The attacks will be looked through first, should be faster due to AB pruning
		std::vector<Move> all_moves = generateAll();
		float best_val = (Board->turn) ? -INFINITY : INFINITY;

		// If no moves, either in checkmate or stalemate
		if (all_moves.empty()){
			if (inCheck(board,turn)){
				return best_val; // In checkmate. return (turn) ? -INFINITY : INFINITY
			}
			return 0; //If not in checkmate, it is a stalemate
		}
		for (Move& mv : all_moves){
			Board->num_boards++;

			//Make the move
			char target = Board->at(mv.sq2); //Holds what is on the target square
			Board->set(mv.sq2, mv.promotion ? mv.promotion : Board->at(mv.sq1)); //If a promotion, set target square to the promoted piece
			Board->set(mv.sq1,'0'); //Set the start square to blank

			//Find the hash value for the board
			unsigned long long int hash = Board->computeHash();

			// Iterator
			auto it = Board->zobrist.find(hash);

			//If the board position has been seen before, its key will exist 
			if (it != Board->zobrist.end()){
				//Undo the move and return its value
				Board->hashes_found++;
				Board->set(mv.sq1, mv.promotion ? (Board->turn ? 'P' : 'p') : Board->at(mv.sq2)); //If a promotion, set target square to the promoted piece
				Board->set(mv.sq2, target);
				return it->second;
			}

			Board->turn = !Board->turn;
			float val = abMinimax(Board, depth-1, alpha, beta);
			Board->turn = !Board->turn;

			//Undo the move
			Board->set(mv.sq1, mv.promotion ? (Board->turn ? 'P' : 'p') : Board->at(mv.sq2)); //If a promotion, set target square to the promoted piece
			Board->set(mv.sq2, target);

			//Add the board to hashtable if not there already
			zobrist[hash] = val;

			//Update best_val and alpha/beta
			if (Board->turn){
				best_val = std::max(best_val, val);
				alpha = std::max(alpha, best_val);
			}else{
				best_val = std::min(best_val, val);
				beta = std::min(beta, best_val);
			}
			if (beta <= alpha){
				break;
			}
		}
		return best_val;
	}
}

//minimax with ab pruning, but it only calculates captures (attacks), and doesn't have a fixed depth
float Minimax::abMinimaxAttacks(Chess &Board, float alpha, float beta){
	float curr_eval = Board->eval();
	if (Board->turn){
		alpha = std::max(alpha, curr_eval);
	}else{
		beta = std::min(beta, curr_eval);
	}
	if (beta <= alpha){
		return curr_eval;
	}

	std::vector<Move> attacks = Board->generateAttacks();

	if (attacks.empty()){
		std::vector<Move> moves = Board->generateMoves();
		if (moves.empty()){
			return (Board->turn) ? -INFINITY : INFINITY;
		}else{
			return curr_eval;
		}
	}else{
		float best_val = (Board->turn) ? -INFINITY : INFINITY;

		for (Move& mv : attacks){
			Board->num_boards++;
			//Make the move
			char target = Board->at(mv.sq2); //Holds what is on the target square
			Board->set(mv.sq2, mv.promotion ? mv.promotion : Board->at(mv.sq1)); //If a promotion, set target square to the promoted piece
			Board->set(mv.sq1,'0'); //Set the start square to blank

			//Find the hash value for the board
			unsigned long long int hash = Board->computeHash();

			// Iterator
			auto it = Board->zobrist.find(hash);

			//If the board position has been seen before, its key will exist 
			if (it != Board->zobrist.end()){
				//Undo the move and return its value
				Board->hashes_found++;
				Board->set(mv.sq1, mv.promotion ? (Board->turn ? 'P' : 'p') : Board->at(mv.sq2)); //If a promotion, set target square to the promoted piece
				Board->set(mv.sq2, target);
				return it->second;
			}

			//if key doesn't exist
			Board->turn = !Board->turn;
			float val = abMinimaxAttacks(Board, alpha, beta);
			Board->turn = !Board->turn;

			//Undo the move
			Board->set(mv.sq1, mv.promotion ? (Board->turn ? 'P' : 'p') : Board->at(mv.sq2)); //If a promotion, set target square to the promoted piece
			Board->set(mv.sq2, target);

			//Add the board
			zobrist[hash] = val;

			//Update best_val and alpha/beta
			if (Board->turn){
				best_val = std::max(best_val, val);
				alpha = std::max(alpha, best_val);
			}else{
				best_val = std::min(best_val, val);
				beta = std::min(beta, best_val);
			}
			if (beta <= alpha){
				break;
			}
		}
		return best_val;
	}
}

Chess::Chess(Variant p_board){
	piece_value = {{'P',1},{'p',-1},{'N',2.95},{'n',-2.95},{'B',3},{'b',-3},{'R',5},{'r',-5},{'Q',9},{'q',-9},{'K',10000},{'k',-10000}, {'0',0}};
	num_boards = 0;
	hashes_found = 0;
	initTable();
	mt.seed(std::time(0));
	setBoard(p_board);
}

void Chess::setBoard(Variant p_board){
	char temp[8][8] = p_board;
	for (int i=0;i<8;i++){
		for (int j=0;j<8;j++){
			set(i,j,temp[i][j]);
		}
	}
}

char Chess::at(const Square sq) const{
	return board[sq.x][sq.y];
}

char Chess::at(const unsigned int i, const unsigned int j) const{
	return board[i][j];
}

void Chess::set(const Square sq, const char piece){
	board[sq.x][sq.y] = piece;
}

// Static board evaluation
float Chess::eval() const{
	float count = 0;
	float val;
	for (int i=0;i<8;i++){
		for (int j=0;j<8;j++){
			val = piece_value[at(i,j)];
			if (at(i,j) == 'P'){
				val *= pow(1.05,(j-1));
			}else if (at(i,j) == 'p'){
				val *= pow(1.05,(6-j));
			}else{
				if (i >= 2 && i <= 5){
					val *= 1.05;
				}
				if (i >= 3 && i <= 4){
					val *= 1.05;
				}
				if (j >= 2 && j <= 5){
					val *= 1.05;
				}
				if (j >= 3 && j <= 4){
					val *= 1.05;
				}
			}
			count += val;
		}
	}
	return count;
}

// Returns an index for piece between 0 and 11
int Chess::indexOf(const char piece) {
	if (piece == 'P'){
		return 0;
	}else if (piece == 'N'){
		return 1;
	}else if (piece == 'B'){
		return 2;
	}else if (piece == 'R'){
		return 3;
	}else if (piece == 'Q'){
		return 4;
	}else if (piece == 'K'){
		return 5;
	}else if (piece == 'p'){
		return 6;
	}else if (piece == 'n'){
		return 7;
	}else if (piece == 'b'){
		return 8;
	}else if (piece == 'r'){
		return 9;
	}else if (piece == 'q'){
		return 10;
	}else if (piece == 'k'){
		return 11;
	}else{
		return -1;
	}
}

// Generates a random u_int64 using mt19937
unsigned long long int Chess::randomInt(){
	std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
	return dist(mt);
}

// Initializes the zobrist hashtable
void Chess::initTable(){
    for (int i = 0; i<8; i++)
    	for (int j = 0; j<8; j++)
        	for (int k = 0; k<12; k++)
          		ZobristTable[i][j][k] = randomInt();
}
 
// Computes the hash value of a given board
unsigned long long int Chess::computeHash(){
    unsigned long long int h = 0;
    for (int i = 0; i<8; i++){
        for (int j = 0; j<8; j++){
            if (at(i,j)!='0'){
                h ^= ZobristTable[i][j][indexOf(at(i,j))];
            }
        }
    }
    return h;
}

// Checks if a generated move is valid, and if so it adds the move to `moves`
bool Chess::tryAddMove(const Move &mv, std::vector<Move> &moves){
	if (mv.sq2.x > 7 || mv.sq2.x < 0 || mv.sq2.y > 7 || mv.sq2.y < 0 || at(sq2) != '0'){
		return false;
	}
	if(isValidMove(board,mv,turn)){
		moves.push_back(Move(mv));
		return true;
	}
	return false;
}

// Checks if a generated attack is valid, and if so it adds the move to `attacks`
bool Chess::tryAddAttack(char board[8][8], const Move &mv, const bool turn, std::vector<Move> &attacks){
	if (mv.sq2.x > 7 || mv.sq2.x < 0 || mv.sq2.y > 7 || mv.sq2.y < 0){
		return false;
	}
	if((isLower(board[mv.sq2.x][mv.sq2.y]) && turn) || (isUpper(board[mv.sq2.x][mv.sq2.y]) && !turn)){
		if (isValidMove(board,mv,turn)){ 
			attacks.push_back(Move(mv));
			return true;
		}
	}
	return false;
}

// Tests if making the move puts the player in check
bool Chess::isValidMove(char board[8][8], const Move &mv, const bool turn){
	//Make the move
	// findKing(board,turn);
	char target = board[mv.sq2.x][mv.sq2.y]; //Holds what is on the target square
	// std::cout << "Target: " << target << std::endl;
	board[mv.sq2.x][mv.sq2.y] = board[mv.sq1.x][mv.sq1.y];
	board[mv.sq1.x][mv.sq1.y] = '0';

	bool valid = (inCheck(board, turn)) ? false : true;

	// if (!valid){
	// 	std::cout << "In Check:" << turn << "\n";
	// 	printBoard(board);
	// }
	//Undo the move
	board[mv.sq1.x][mv.sq1.y] = (mv.promotion) ? (Board->turn ? 'P' : 'p') : board[mv.sq2.x][mv.sq2.y]; //If a promotion, turn the piece back into a pawn
	board[mv.sq2.x][mv.sq2.y] = target;

	return valid;
}

// Returns if a char is uppercase
bool Chess::isUpper(const char letter){
	return (letter >= 65 && letter <= 90) ? true : false;
}

// Returns if a char is lowercase
bool Chess::isLower(const char letter){
	return (letter >= 97 && letter <= 122) ? true : false;
}

std::vector<Move> Chess::generateAll(){
	std::vector<Move> all_moves = generateAttacks(board,turn);
	std::vector<Move> moves = generateMoves(board,turn);
	all_moves.insert(all_moves.end(), moves.begin(), moves.end());
	return all_moves;
}

// Generates all non-captures
std::vector<Move> Chess::generateMoves(char board[8][8], const bool turn){
	char piece;
	std::vector<Move> moves;
	if (turn){
		for (int i=0;i<8;i++){
			for (int j=0;j<8;j++){
				piece = board[i][j];
				if (piece == 'P'){
					generatePawnMove(board, Square(i,j), turn, moves);
				}else if (piece == 'N'){
					generateKnightMove(board, Square(i,j), turn, moves);
				}else if (piece == 'B'){
					generateBishopMove(board, Square(i,j), turn, moves);
				}else if (piece == 'R'){
					generateRookMove(board, Square(i,j), turn, moves);
				}else if (piece == 'Q'){
					generateQueenMove(board, Square(i,j), turn, moves);
				}else if (piece == 'K'){
					generateKingMove(board, Square(i,j), turn, moves);
				}
			}
		}
	}else{
		for (int i=0;i<8;i++){
			for (int j=0;j<8;j++){
				piece = board[i][j];
				if (piece == 'p'){
					generatePawnMove(board, Square(i,j), turn, moves);
				}else if (piece == 'n'){
					generateKnightMove(board, Square(i,j), turn, moves);
				}else if (piece == 'b'){
					generateBishopMove(board, Square(i,j), turn, moves);
				}else if (piece == 'r'){
					generateRookMove(board, Square(i,j), turn, moves);
				}else if (piece == 'q'){
					generateQueenMove(board, Square(i,j), turn, moves);
				}else if (piece == 'k'){
					generateKingMove(board, Square(i,j), turn, moves);
				}
			}
		}
	}
	return moves;
}

// Generates all captures
std::vector<Move> Chess::generateAttacks(char board[8][8], const bool turn){
	char piece;
	std::vector<Move> attacks;
	if (turn){
		for (int i=0;i<8;i++){
			for (int j=0;j<8;j++){
				piece = board[i][j];
				if (piece == 'P'){
					generatePawnAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'N'){
					generateKnightAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'B'){
					generateBishopAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'R'){
					generateRookAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'Q'){
					generateQueenAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'K'){
					generateKingAttack(board, Square(i,j), turn, attacks);
				}
			}
		}
	}else{
		for (int i=0;i<8;i++){
			for (int j=0;j<8;j++){
				piece = board[i][j];
				if (piece == 'p'){
					generatePawnAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'n'){
					generateKnightAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'b'){
					generateBishopAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'r'){
					generateRookAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'q'){
					generateQueenAttack(board, Square(i,j), turn, attacks);
				}else if (piece == 'k'){
					generateKingAttack(board, Square(i,j), turn, attacks);
				}
			}
		}
	}
	return attacks;
}

// Adds legal pawn moves to `moves` vector 
void Chess::generatePawnMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves){
	// std::cout << "Pawn move called" << std::endl;
	Square target_sq(sq); //Temporary target square for move
	int dir = (turn) ? 1 : -1; //Direction pawn is moving (white moves +, black moves -)
	const char *promotions = (turn) ? "QNRB" : "qnrb";
	//Move 1 ahead
	target_sq.y += dir;

	//If the pawn is about to promote
	if ((turn && sq.y==6) || (!turn && sq.y==1)){
		if (tryAddMove(board, Move(sq, target_sq, promotions[0]), turn, moves)){
			for(int i=1;i<4;i++){
				moves.push_back(Move(sq, target_sq, promotions[i]));
			}
		}
	//If the pawn is not about to promote
	}else if (tryAddMove(board, Move(sq, target_sq, 0), turn, moves) && ((turn && sq.y==1) || (!turn && sq.y==6))){
		//If moving 1 ahead is a legal move, and the pawn is on it's starting rank, try moving 1 more ahead (2 ahead)
		target_sq.y += dir;
		tryAddMove(board, Move(sq, target_sq, 0), turn, moves);
	}
}

// Adds legal pawn attacks to `attacks` vector
void Chess::generatePawnAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks){
	// std::cout << "Pawn attack called" << std::endl;
	Square target_sq(sq); //Temporary target square for move
	int dir = (turn) ? 1 : -1; //Direction pawn is moving (white moves +, black moves -)
	const char *promotions = (turn) ? "QNRB" : "qnrb";

	//Move 1 ahead
	target_sq.y += dir;

	//If the pawn is about to promote
	if ((turn && sq.y==6) || (!turn && sq.y==1)){
		//Attack x - 1
		target_sq.x -= 1;
		if (tryAddAttack(board, Move(sq, target_sq, promotions[0]), turn, attacks)){
			for(int i=1;i<4;i++){
				attacks.push_back(Move(sq, target_sq, promotions[i]));
			}
		}

		//Attack x + 1
		target_sq.x += 2;
		if (tryAddAttack(board, Move(sq, target_sq, promotions[0]), turn, attacks)){
			for(int i=1;i<4;i++){
				attacks.push_back(Move(sq, target_sq, promotions[i]));
			}
		}
	//If the pawn is not about to promote
	}else{
		//Attack x - 1
		target_sq.x -= 1;
		tryAddAttack(board, Move(sq, target_sq, 0), turn, attacks);

		//Attack x + 1
		target_sq.x += 2;
		tryAddAttack(board, Move(sq, target_sq, 0), turn, attacks);
	}
}

// Adds legal knight moves to `moves` vector
void Chess::generateKnightMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves){
	// std::cout << "knight move called" << std::endl;
	Square targets[8] = {Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)};
	for (int i=0;i<8;i++){
		tryAddMove(board, Move(sq, targets[i], 0), turn, moves);
	}
}

// Adds legal knight attacks to `attacks` vector
void Chess::generateKnightAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks){
	// std::cout << "Knight attack called" << std::endl;
	Square targets[8] = {Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)};
	for (int i=0;i<8;i++){
		tryAddAttack(board, Move(sq, targets[i], 0), turn, attacks);
	}
}

// Adds legal bishop moves to `moves` vector
void Chess::generateBishopMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves){
	// std::cout << "Bishop move called" << std::endl;
	Square dirs[4] = {Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (true){
			if(!tryAddMove(board, Move(sq, target, 0), turn, moves)){
				break;
			}
			target = target + dirs[i];
		}
	}
}

// Adds legal bishop attacks to `attacks` vector
void Chess::generateBishopAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks){
	// std::cout << "Bishop attack called" << std::endl;
	Square dirs[4] = {Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if((isUpper(board[target.x][target.y]) && turn) || (isLower(board[target.x][target.y]) && !turn)){
				break;
			}else if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
			}else{
				tryAddAttack(board, Move(sq, target, 0), turn, attacks);
				break;
			}
		}
	}
}

// Adds legal rook moves to `moves` vector
void Chess::generateRookMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves){
	// std::cout << "Rook move called" << std::endl;
	Square dirs[4] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (true){
			if(!tryAddMove(board, Move(sq, target, 0), turn, moves)){
				break;
			}
			target = target + dirs[i];
		}
	}
}

// Adds legal rook attacks `attacks` vector
void Chess::generateRookAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks){
	// std::cout << "Rook attack called" << std::endl;
	Square dirs[4] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if((isUpper(board[target.x][target.y]) && turn) || (isLower(board[target.x][target.y]) && !turn)){
				break;
			}
			if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
				continue;
			}else{
				tryAddAttack(board, Move(sq, target, 0), turn, attacks);
				break;
			}
		}
	}
}

// Adds legal queen moves to `moves` vector
void Chess::generateQueenMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves){
	// std::cout << "Queen move called" << std::endl;
	Square dirs[8] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1), Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<8;i++){
		target = dirs[i] + sq;
		while (true){
			if(!tryAddMove(board, Move(sq, target, 0), turn, moves)){
				break;
			}
			target = target + dirs[i];
		}
	}
}

// Adds legal queen attacks to `attacks` vector
void Chess::generateQueenAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks){
	// std::cout << "Queen attack called" << std::endl;
	Square dirs[8] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1), Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<8;i++){
		target = dirs[i] + sq;
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if((isUpper(board[target.x][target.y]) && turn) || (isLower(board[target.x][target.y]) && !turn)){
				break;
			}
			if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
				continue;
			}else{
				tryAddAttack(board, Move(sq, target, 0), turn, attacks);
				break;
			}
		}
	}
}

// Adds legal king moves to `moves` vector
void Chess::generateKingMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves){
	// std::cout << "King move called" << std::endl;
	Square targets[8] = {Square(sq.x - 1, sq.y - 1), Square(sq.x - 1, sq.y), Square(sq.x - 1, sq.y + 1), Square(sq.x, sq.y - 1), Square(sq.x, sq.y + 1), Square(sq.x + 1, sq.y - 1), Square(sq.x + 1, sq.y), Square(sq.x + 1, sq.y + 1)};
	for (int i=0;i<8;i++){
		tryAddMove(board, Move(sq, targets[i], 0), turn, moves);
	}
}

// Adds legal king attacks to `attacks` vector
void Chess::generateKingAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks){
	// std::cout << "King attack called" << std::endl;
	Square targets[8] = {Square(sq.x - 1, sq.y - 1), Square(sq.x - 1, sq.y), Square(sq.x - 1, sq.y + 1), Square(sq.x, sq.y - 1), Square(sq.x, sq.y + 1), Square(sq.x + 1, sq.y - 1), Square(sq.x + 1, sq.y), Square(sq.x + 1, sq.y + 1)};
	for (int i=0;i<8;i++){
		tryAddAttack(board, Move(sq, targets[i], 0), turn, attacks);
	}
}

// Returns whether `turn` player is in check
bool Chess::inCheck(const char board[8][8], const bool turn){
	Square sq = findKing(board,turn);
	if (sq == Square(-1,-1)){
		std::cout << "ff\n";
		return true;
	}
	int offset = (turn) ? 32 : 0; //Add to convert to non-turn color piece
	Square dirs[8] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1), Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<8;i++){
		target = sq + dirs[i];
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
				continue;
			}else if ((isUpper(board[target.x][target.y]) && turn) || (isLower(board[target.x][target.y]) && !turn)){
				break;
			}else{ // Target piece must be a different color than `turn`
				if (i < 4){
					if((board[target.x][target.y] == 'R' + offset) || (board[target.x][target.y] == 'Q' + offset)){
						// std::cout << "Check rook/queen\n";
						return true;
					}else{
						break;
					}
				}else{
					if((board[target.x][target.y] == 'B' + offset) || (board[target.x][target.y] == 'Q' + offset)){
						// std::cout << "Check bishop/queen\n";
						return true;	
					}else{
						break;
					}
				}
				// std::cout << dirs[i].x << "," <<dirs[i].y << std::endl;
				// std::cout << target.x << "," << target.y << std::endl;
			}
		}
	}
	// Knight Checks
	Square targets[8] = {Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)};
	for (int i=0;i<8;i++){
		if(targets[i].x < 0 || targets[i].x > 7 || targets[i].y < 0 || targets[i].y > 7){
			continue;
		}
		if(board[targets[i].x][targets[i].y] == 'N' + offset){
			// std::cout << "Check knight\n";
			return true;
		}
	}
	// Pawn Checks
	Square psq[2];
	if(turn){
		psq[0] = Square(sq.x+1,sq.y+1);
		psq[1] = Square(sq.x-1,sq.y+1);
	}else{
		psq[0] = Square(sq.x+1,sq.y-1);
		psq[1] = Square(sq.x-1,sq.y-1);
	}
	for (int i=0;i<2;i++){
		if(psq[i].x < 0 || psq[i].x > 7 || psq[i].y < 0 || psq[i].y > 7){
			continue;
		}
		if(board[psq[i].x][psq[i].y] == 'P' + offset){
			// std::cout << "Check knight\n";
			return true;
		}
	}
	return false;
}

// Returns the square the king is on for `turn` player
Square Chess::findKing(const char board[8][8], const bool turn){
	char king = (turn) ? 'K' : 'k';
	for (int i=0;i<8;i++){
		for (int j=0;j<8;j++){
			if (board[i][j]==king){
				return Square(i,j);
			}
		}
	}
	std::cout << "King not Found\n";
	printBoard(board);
	return Square(-1,-1);
}

void Chess::printBoard(const char board[8][8]){
	for (int j=7;j>=0;j--){
        for(int i=0;i<8;i++){
            std::cout << board[i][j];
        }
        std::cout << std::endl; 
    }
	std::cout << std::endl; 
}

//Summary: Default Constructor of Move class
//Parameters
//Usage: Move()
Move::Move(){
	sq1 = Square();
	sq2 = Square();
	promotion = 0;
}

//Summary: Constructor of Move class
//Parameters
//param1(TYPE:const Square reference, PURPOSE:Initial square of move, EX: Square(7,4) is h5 ) 
//param2(TYPE:const Square reference, PURPOSE:Final square of move, EX:Square(0,6) is a7 ) 
//Usage: Move(square1, square2)
Move::Move(const Square &p_sq1, const Square &p_sq2, char p_promotion){
	sq1 = p_sq1;
	sq2 = p_sq2;
	promotion = p_promotion;
}

Move::Move(const char *move){
	sq1 = Square(move[0] - 'a', move[1] - '1');
	sq2 = Square(move[2] - 'a', move[3] - '1');
	promotion = 0;
}

//Summary: Assignment operator for the move class
//Parameters
//param1(TYPE:const Move reference, PURPOSE:Move to be copied, EX: Move(sqaure1, square2) is the piece on square1 moves to square2) 
//Usage: newMove = oldMove
Move Move::operator=(const Move &p_mv){
	sq1 = p_mv.sq1;
	sq2 = p_mv.sq2;
	promotion = p_mv.promotion;
	return *this;
}

void Move::printMove() const{
	std::cout << "(" << char(sq1.x + 97) << "," << sq1.y+1 << ") -> (" << char(sq2.x + 97) << "," << sq2.y+1 << ")\n";
}

//Summary: Default Constructor of Square class
//Parameters
//Usage: Square()
Square::Square(){
	x = 0;
	y = 0;
}

//Summary: Constructor of Square class
//Parameters
//param1(TYPE:const int, PURPOSE:x index of board, EX:x==0 is the 'a' file; x==2 is the 'c' file; etc. ) 
//param2(TYPE:const int, PURPOSE:y index of board, EX:y==0 is the 1st rank; x==2 is the 3rd rank; etc. ) 
//Usage: Square(1,2)
Square::Square(const int &p_x, const int &p_y){
	x = p_x;
	y = p_y;
}

//Summary: Assignment operator for the Square class
//Parameters
//param1(TYPE:const Square reference, PURPOSE:Square that will be copied, EX: Square(3,1) is d2 )
//Usage: newSq = oldSq
Square Square::operator=(const Square &p_sq){
	x = p_sq.x;
	y = p_sq.y;
	return *this;
}

//Summary: Addition operator for the Square class
//Parameters
//param1(TYPE:const Square reference, PURPOSE:Square that will be added, EX: Square(3,1) is d2 )
//Usage: newSq += oldSq
Square Square::operator+(const Square &p_sq) const{
	return Square(x+p_sq.x, y+p_sq.y);
}

bool Square::operator==(const Square &p_sq) const{
	return (x == p_sq.x && y == p_sq.y);
}