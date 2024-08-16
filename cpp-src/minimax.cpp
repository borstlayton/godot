#include "minimax.h"
// #include <godot_cpp/core/class_db.hpp>
#include <iostream>

// using namespace godot;
using namespace std;

// void Minimax::_bind_methods() {
//     ClassDB::bind_method(D_METHOD("get_amplitude"), &Minimax::get_amplitude);
// 	ClassDB::bind_method(D_METHOD("set_amplitude", "p_amplitude"), &Minimax::set_amplitude);
// 	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");
//     ClassDB::bind_method(D_METHOD("get_speed"), &Minimax::get_speed);
// 	ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &Minimax::set_speed);
// 	ADD_PROPERTY("Minimax", PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
// }

Minimax::Minimax() {
	// Initialize any variables here.
	initTable();
	piece_value['P'] = 1;
	piece_value['N'] = 2.95;
	piece_value['B'] = 3;
	piece_value['R'] = 5;
	piece_value['Q'] = 9;
	piece_value['K'] = 10000;
	piece_value['p'] = -1;
	piece_value['n'] = -2.95;
	piece_value['b'] = -3;
	piece_value['r'] = -5;
	piece_value['q'] = -9;
	piece_value['k'] = -10000;
	piece_value['0'] = 0;
	mt.seed(8675309); // Jenny Jenny
}

Minimax::~Minimax() {
	// Add your cleanup here.
}

// void Minimax::_process(double delta) {
// 	time_passed += speed * delta;

// 	Vector2 new_position = Vector2(
//         amplitude + (amplitude * sin(time_passed * 2.0)),
// 		amplitude + (amplitude * cos(time_passed * 1.5))
//     );

// 	set_position(new_position);
// }

// Returns the best move
Move Minimax::findBest(char board[8][8], bool turn, unsigned int depth){
	//hash table for zobrist hashing
	std::unordered_map<unsigned long long int, float> zobrist;
	
	//Generate all moves, with attacks first
	vector<Move> all_moves = generateAttacks(board,turn);
	vector<Move> moves = generateMoves(board,turn);
	all_moves.insert(all_moves.end(), moves.begin(), moves.end());

	float best_val = (turn) ? -INFINITY : INFINITY;
	Move best_move;

	for (Move& mv : all_moves){
		//Make the move
		char target = board[mv.sq2.x][mv.sq2.y]; //Holds what is on the target square
		board[mv.sq2.x][mv.sq2.y] = (mv.promotion) ? mv.promotion : board[mv.sq1.x][mv.sq1.y]; //If a promotion, promote the piece
		board[mv.sq1.x][mv.sq1.y] = '0';

		float val = abMinimax(board, depth-1, !turn, -INFINITY, INFINITY, zobrist);

		//Undo the move
		board[mv.sq1.x][mv.sq1.y] = (mv.promotion) ? (turn ? 'P' : 'p') : board[mv.sq2.x][mv.sq2.y]; //If a promotion, turn the piece back into a pawn
		board[mv.sq2.x][mv.sq2.y] = target;

		//Update best_val and best_move
		if ((turn && val > best_val) || (!turn && val < best_val)){
			best_val = val;
			best_move = mv;
		}
	}
	return best_move;
}

//minimax with ab pruning, and a fixed depth
float Minimax::abMinimax(char board[8][8], int depth, bool turn, float alpha, float beta, std::unordered_map<unsigned long long int, float> zobrist){
	if (depth == 0){
		return abMinimaxAttacks(board, !turn, -INFINITY, INFINITY, zobrist);
	}else{
		float best_val = (turn) ? -INFINITY : INFINITY;
		//The attacks will be looked through first, should be faster due to AB pruning
		vector<Move> all_moves = generateAttacks(board,turn);
		vector<Move> moves = generateMoves(board,turn);
		all_moves.insert(all_moves.end(), moves.begin(), moves.end());

		for (Move& mv : all_moves){
			//Make the move
			char target = board[mv.sq2.x][mv.sq2.y]; //Holds what is on the target square
			board[mv.sq2.x][mv.sq2.y] = (mv.promotion) ? mv.promotion : board[mv.sq1.x][mv.sq1.y]; //If a promotion, promote the piece
			board[mv.sq1.x][mv.sq1.y] = '0';

			//Find the hash value for the board
			unsigned long long int hash = computeHash(board);

			//If the board position has been seen before, its key will exist 
			if (zobrist.find(hash) != zobrist.end()){
				return zobrist[hash];
			}

			float val = abMinimax(board, depth-1, !turn, alpha, beta, zobrist);

			//Add the board 
			zobrist[hash] = val;

			//Undo the move
			board[mv.sq1.x][mv.sq1.y] = (mv.promotion) ? (turn ? 'P' : 'p') : board[mv.sq2.x][mv.sq2.y]; //If a promotion, turn the piece back into a pawn
			board[mv.sq2.x][mv.sq2.y] = target;

			//Update best_val and alpha/beta
			if (turn){
				best_val = max(best_val, val);
				alpha = max(alpha, best_val);
			}else{
				best_val = min(best_val, val);
				beta = min(beta, best_val);
			}
			if (beta <= alpha){
				break;
			}
		}
		return best_val;
	}
}

//minimax with ab pruning, but it only calculates captures (attacks), and doesn't have a fixed depth
float Minimax::abMinimaxAttacks(char board[8][8], bool turn, float alpha, float beta, std::unordered_map<unsigned long long int, float> zobrist){
	std::vector<Move> attacks = generateAttacks(board, turn);
	if (attacks.empty()){
		return eval(board);
	}else{
		float best_val = (turn) ? -INFINITY : INFINITY;
		//The attacks will be looked through first, should be faster due to AB pruning

		for (Move& mv : attacks){
			//Make the move
			char target = board[mv.sq2.x][mv.sq2.y]; //Holds what is on the target square
			board[mv.sq2.x][mv.sq2.y] = (mv.promotion) ? mv.promotion : board[mv.sq1.x][mv.sq1.y]; //If a promotion, promote the piece
			board[mv.sq1.x][mv.sq1.y] = '0';

			//Find the hash value for the board
			unsigned long long int hash = computeHash(board);

			//If the board position has been seen before, its key will exist 
			if (zobrist.find(hash) != zobrist.end()){
				return zobrist[hash];
			}

			float val = abMinimaxAttacks(board, !turn, alpha, beta, zobrist);

			//Add the board 
			zobrist[hash] = val;

			//Undo the move
			board[mv.sq1.x][mv.sq1.y] = (mv.promotion) ? (turn ? 'P' : 'p') : board[mv.sq2.x][mv.sq2.y]; //If a promotion, turn the piece back into a pawn
			board[mv.sq2.x][mv.sq2.y] = target;

			//Update best_val and alpha/beta
			if (turn){
				best_val = max(best_val, val);
				alpha = max(alpha, best_val);
			}else{
				best_val = min(best_val, val);
				beta = min(beta, best_val);
			}
			if (beta <= alpha){
				break;
			}
		}
		return best_val;
	}
}

// Static board evaluation
float Minimax::eval(const char board[8][8]){
	float count = 0;
	for (int i=0;i<8;i++){
		for (int j=0;j<8;j++){
			count += piece_value[board[i][j]];
		}
	}
}

// Returns an index for piece between 0 and 11
int Minimax::indexOf(char piece) {
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
unsigned long long int Minimax::randomInt(){
	std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
	return dist(mt);
}

// Initializes the zobrist hashtable
void Minimax::initTable(){
    for (int i = 0; i<8; i++)
    	for (int j = 0; j<8; j++)
        	for (int k = 0; k<12; k++)
          		ZobristTable[i][j][k] = randomInt();
}
 
// Computes the hash value of a given board
unsigned long long int Minimax::computeHash(const char board[8][8]){
    unsigned long long int h = 0;
    for (int i = 0; i<8; i++){
        for (int j = 0; j<8; j++){
            if (board[i][j]!='0'){
                h ^= ZobristTable[i][j][indexOf(board[i][j])];
            }
        }
    }
    return h;
}

// Checks if a generated move is valid, and if so it adds the move to `moves`
bool Minimax::tryAddMove(char board[8][8], const Move &mv, const bool &turn, vector<Move> moves){
	if (mv.sq2.x > 7 || mv.sq2.x < 0 || mv.sq2.y > 7 || mv.sq2.y < 0){
		return false;
	}
	if(board[mv.sq2.x][mv.sq2.y] == '0' && isValidMove(board,mv,turn)){
		moves.push_back(Move(mv));
		return true;
	}
	return false;
}

// Checks if a generated attack is valid, and if so it adds the move to `attacks`
bool Minimax::tryAddAttack(char board[8][8], const Move &mv, const bool &turn, vector<Move> attacks){
	if (mv.sq2.x > 7 || mv.sq2.x < 0 || mv.sq2.y > 7 || mv.sq2.y < 0){
		return false;
	}
	if(board[mv.sq2.x][mv.sq2.y] == '0'){
		return false;
	}
	// If the target piece is the same color as whose turn it is, return false
	if (isUpper(board[mv.sq2.x][mv.sq2.y]) == turn){
		return false;
	}
	if (isValidMove(board,mv,turn)){
		attacks.push_back(Move(mv));
		return true;
	}
	return false;
}

// Tests if making the move puts the player in check
bool Minimax::isValidMove(char board[8][8], const Move &mv, const bool &turn){
	//Make the move
	char target = board[mv.sq2.x][mv.sq2.y]; //Holds what is on the target square
	board[mv.sq2.x][mv.sq2.y] = board[mv.sq1.x][mv.sq1.y];
	board[mv.sq1.x][mv.sq1.y] = '0';

	bool valid = (inCheck(board, turn)) ? false : true;

	board[mv.sq1.x][mv.sq1.y] = board[mv.sq2.x][mv.sq2.y];
	board[mv.sq2.x][mv.sq2.y] = target;

	return valid;
}

// Returns if a char is uppercase
bool Minimax::isUpper(const char &letter){
	return (letter >= 65 && letter <= 90) ? true : false;
}

// Returns if a char is lowercase
bool Minimax::isLower(const char &letter){
	return (letter >= 97 && letter <= 122) ? true : false;
}

// Generates all non-captures
vector<Move> Minimax::generateMoves(char board[8][8], const bool turn){
	char piece;
	vector<Move> moves;
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
vector<Move> Minimax::generateAttacks(char board[8][8], const bool turn){
	char piece;
	vector<Move> attacks;
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
void Minimax::generatePawnMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves){
	Square target_sq(sq); //Temporary target square for move
	int dir = (turn) ? 1 : -1; //Direction pawn is moving (white moves +, black moves -)
	char *promotions = (turn) ? "QNRB" : "qnrb";
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
	}else if (tryAddMove(board, Move(sq, target_sq), turn, moves) && ((turn && sq.y==1) || (!turn && sq.y==6))){
		//If moving 1 ahead is a legal move, and the pawn is on it's starting rank, try moving 1 more ahead (2 ahead)
		target_sq.y += dir;
		tryAddMove(board, Move(sq, target_sq), turn, moves);
	}
}

// Adds legal pawn attacks to `attacks` vector
void Minimax::generatePawnAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks){
	Square target_sq(sq); //Temporary target square for move
	int dir = (turn) ? 1 : -1; //Direction pawn is moving (white moves +, black moves -)
	char *promotions = (turn) ? "QNRB" : "qnrb";

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
		tryAddAttack(board, Move(sq, target_sq), turn, attacks);

		//Attack x + 1
		target_sq.x += 2;
		tryAddAttack(board, Move(sq, target_sq), turn, attacks);
	}
}

// Adds legal knight moves to `moves` vector
void Minimax::generateKnightMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves){
	Square targets[8] = {Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)};
	for (int i=0;i<8;i++){
		tryAddMove(board, Move(sq, targets[i]), turn, moves);
	}
}

// Adds legal knight attacks to `attacks` vector
void Minimax::generateKnightAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks){
	Square targets[8] = {Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)};
	for (int i=0;i<8;i++){
		tryAddAttack(board, Move(sq, targets[i]), turn, attacks);
	}
}

// Adds legal bishop moves to `moves` vector
void Minimax::generateBishopMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves){
	Square dirs[4] = {Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (true){
			if(!tryAddMove(board, Move(sq, dirs[i]), turn, moves)){
				break;
			}
			target = target + dirs[i];
		}
	}
}

// Adds legal bishop attacks to `attacks` vector
void Minimax::generateBishopAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks){
	Square dirs[4] = {Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if(isUpper(board[target.x][target.y])==turn){
				break;
			}else if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
			}else{
				tryAddAttack(board, Move(sq, target), turn, attacks);
			}
		}
	}
}

// Adds legal rook moves to `moves` vector
void Minimax::generateRookMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves){
	Square dirs[4] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (true){
			if(!tryAddMove(board, Move(sq, dirs[i]), turn, moves)){
				break;
			}
			target = target + dirs[i];
		}
	}
}

// Adds legal rook attacks `attacks` vector
void Minimax::generateRookAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks){
	Square dirs[4] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1)};
	Square target;
	for (int i=0;i<4;i++){
		target = dirs[i] + sq;
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if(isUpper(board[target.x][target.y])==turn){
				break;
			}else if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
			}else{
				tryAddAttack(board, Move(sq, target), turn, attacks);
			}
		}
	}
}

// Adds legal queen moves to `moves` vector
void Minimax::generateQueenMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves){
	Square dirs[8] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1), Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<8;i++){
		target = dirs[i] + sq;
		while (true){
			if(!tryAddMove(board, Move(sq, dirs[i]), turn, moves)){
				break;
			}
			target = target + dirs[i];
		}
	}
}

// Adds legal queen attacks to `attacks` vector
void Minimax::generateQueenAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks){
	Square dirs[8] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1), Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<8;i++){
		target = dirs[i] + sq;
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if(isUpper(board[target.x][target.y])==turn){
				break;
			}else if (board[target.x][target.y] == '0'){
				target = target + dirs[i];
			}else{
				tryAddAttack(board, Move(sq, target), turn, attacks);
			}
		}
	}
}

// Adds legal king moves to `moves` vector
void Minimax::generateKingMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves){
	Square targets[8] = {Square(sq.x - 1, sq.y - 1), Square(sq.x - 1, sq.y), Square(sq.x - 1, sq.y + 1), Square(sq.x, sq.y - 1), Square(sq.x, sq.y + 1), Square(sq.x + 1, sq.y - 1), Square(sq.x + 1, sq.y), Square(sq.x + 1, sq.y + 1)};
	for (int i=0;i<8;i++){
		tryAddMove(board, Move(sq, targets[i]), turn, moves);
	}
}

// Adds legal king attacks to `attacks` vector
void Minimax::generateKingAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks){
	Square targets[8] = {Square(sq.x - 1, sq.y - 1), Square(sq.x - 1, sq.y), Square(sq.x - 1, sq.y + 1), Square(sq.x, sq.y - 1), Square(sq.x, sq.y + 1), Square(sq.x + 1, sq.y - 1), Square(sq.x + 1, sq.y), Square(sq.x + 1, sq.y + 1)};
	for (int i=0;i<8;i++){
		tryAddAttack(board, Move(sq, targets[i]), turn, attacks);
	}
}

// Returns whether `turn` player is in check
bool Minimax::inCheck(const char board[8][8], const bool &turn){
	Square sq = findKing(board,turn);
	Square dirs[8] = {Square(1, 0), Square(-1, 0), Square(0, -1), Square(0, 1), Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)};
	Square target;
	for (int i=0;i<8;i++){
		target = sq + dirs[i];
		while (target.x >= 0 && target.x <= 7 && target.y >= 0 && target.y <= 7){
			if(isLower(board[target.x][target.y])){
				return true;
			}
			target = target + dirs[i];
		}
	}
	Square targets[8] = {Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)};
	for (int i=0;i<8;i++){
		if(isLower(board[targets[i].x][targets[i].y])){
			return true;
		}
	}
	return false;
}

// Returns the square the king is on for `turn` player
Square Minimax::findKing(const char board[8][8], const bool &turn){
	char king = (turn) ? 'K' : 'k';
	for (int i=0;i<8;i++){
		for (int j=0;j<8;j++){
			if (board[i][j]==king){
				return Square(i,j);
			}
		}
	}
	std::cout << "King not Found\n";
	return Square(-1,-1);
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
Move::Move(const Square &p_sq1, const Square &p_sq2, char p_promotion = 0){
	sq1 = p_sq1;
	sq2 = p_sq2;
	promotion = p_promotion;
}

//Summary: Assignment operator for the move class
//Parameters
//param1(TYPE:const Move reference, PURPOSE:Move to be copied, EX: Move(sqaure1, square2) is the piece on square1 moves to square2) 
//Usage: newMove = oldMove
Move Move::operator=(const Move &p_mv){
	sq1 = p_mv.sq1;
	sq2 = p_mv.sq2;
	promotion = p_mv.promotion;
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
}

//Summary: Addition operator for the Square class
//Parameters
//param1(TYPE:const Square reference, PURPOSE:Square that will be added, EX: Square(3,1) is d2 )
//Usage: newSq += oldSq
Square Square::operator+(const Square &p_sq){
	x += p_sq.x;
	y += p_sq.y;
}
