#include "minimax.h"
#include <godot_cpp/core/class_db.hpp>
#include <iostream>
#include <random>
#include <vector>

using namespace godot;

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
Square::Square(const int p_x, const int p_y){
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
//param1(TYPE:const Square reference, PURPOSE:Square that will be added to, EX: Square(3,1) is d2 )
//param1(TYPE:const Square reference, PURPOSE:Square that will be added, EX: Square(3,1) is d2 )
//Usage: newSq += oldSq
Square Square::operator+(const Square &p_sq1, const Square &p_sq2){
	p_sq1.x += p_sq2.x;
	p_sq1.y += p_sq2.y;
}

//Summary: Default Constructor of Move class
//Parameters
//Usage: Move()
Move::Move(){
	sq1 = Square();
	sq2 = Square();
}

//Summary: Constructor of Move class
//Parameters
//param1(TYPE:const Square reference, PURPOSE:Initial square of move, EX: Square(7,4) is h5 ) 
//param2(TYPE:const Square reference, PURPOSE:Final square of move, EX:Square(0,6) is a7 ) 
//Usage: Move(square1, square2)
Move::Move(const Square &p_sq1, const Square &p_sq2){
	sq1 = p_sq1;
	sq2 = p_sq2;
}

//Summary: Assignment operator for the move class
//Parameters
//param1(TYPE:const Move reference, PURPOSE:Move to be copied, EX: Move(sqaure1, square2) is the piece on square1 moves to square2) 
//Usage: newMove = oldMove
Move Move::operator=(const Move &p_mv){
	sq1 = p_mv.sq1;
	sq2 = p_mv.sq2;
}

void Minimax::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_amplitude"), &Minimax::get_amplitude);
	ClassDB::bind_method(D_METHOD("set_amplitude", "p_amplitude"), &Minimax::set_amplitude);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude"), "set_amplitude", "get_amplitude");
    ClassDB::bind_method(D_METHOD("get_speed"), &Minimax::get_speed);
	ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &Minimax::set_speed);
	ADD_PROPERTY("Minimax", PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,20,0.01"), "set_speed", "get_speed");
}

Minimax::Minimax() {
	// Initialize any variables here.
	initTable();
}

Minimax::~Minimax() {
	// Add your cleanup here.
}

void Minimax::_process(double delta) {
	time_passed += speed * delta;

	Vector2 new_position = Vector2(
        amplitude + (amplitude * sin(time_passed * 2.0)),
		amplitude + (amplitude * cos(time_passed * 1.5))
    );

	set_position(new_position);
}

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

unsigned long long int Minimax::randomInt() const {
	std::uniform_int_distribution<unsigned long long int> dist(0, UINT64_MAX);
	return dist(mt)
}

// Initializes the table
void Minimax::initTable()
{
    for (int i = 0; i<8; i++)
    	for (int j = 0; j<8; j++)
        	for (int k = 0; k<12; k++)
          		ZobristTable[i][j][k] = randomInt();
}
 
// Computes the hash value of a given board
unsigned long long int Minimax::computeHash(char board[8][8])
{
    unsigned long long int h = 0;
    for (int i = 0; i<8; i++)
    {
        for (int j = 0; j<8; j++)
        {
            if (board[i][j]!='0')
            {
                int piece = indexOf(board[i][j]);
                h ^= ZobristTable[i][j][piece];
            }
        }
    }
    return h;
}

bool Minimax::tryMove(const char board[8][8], const Move mv, const bool turn) const{
	if (mv.sq2.x > 7 || mv.sq2.x < 0 || mv.sq2.y > 7 || mv.sq2.y < 0){
		return false;
	}else if(board[mv.sq2.x][mv.sq2.y] != '0'){
		return false;
	}else{
		return true;
	}
}

bool Minimax::tryMove(const char board[8][8], const Move mv, const bool turn) const{
	if (mv.sq2.x > 7 || mv.sq2.x < 0 || mv.sq2.y > 7 || mv.sq2.y < 0){
		return false;
	}
	if(board[mv.sq2.x][mv.sq2.y] == '0'){
		return true;
	}else{
		return false;
	}
}

bool Minimax::tryAttack(const char board[8][8], const Move mv, const bool turn) const{
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
	return true;
}

bool Minimax::isValidMove(char board[8][8], const Move &mv, const bool turn){
	//Make the move
	char target = board[mv.sq2.x][mv.sq2.y]; //Holds what is on the target square
	board[mv.sq2.x][mv.sq2.y] = board[mv.sq1.x][mv.sq1.y];
	board[mv.sq1.x][mv.sq1.y] = '0';

	bool valid (inCheck(board, turn)) ? false : true;

	board[mv.sq1.x][mv.sq1.y] = board[mv.sq2.x][mv.sq2.y];
	board[mv.sq2.x][mv.sq2.y] = target;

	return valid;
}

bool Minimax::isUpper(const char letter){
	return (letter >= 65 && letter <= 90) ? true : false;
}

bool Minimax::isLower(const char letter){
	return (letter >= 97 && letter <= 122) ? true : false;
}

void Minimax::generateMoves(const char board[8][8], const bool turn){
	char piece;
	vector<Move> moves;
	if (turn){
		for (int i=0;i<8;i++){
			for (int j=0,j<8;j++){
				piece = board[i][j];
				if (piece == 'P'){
					generatePawnMove(Square(i,j), turn, moves);
				}else if (piece == 'N'){
					generatePawnMove(Square(i,j), turn);
				}else if (piece == 'B'){
					generatePawnMove(Square(i,j), turn);
				}else if (piece == 'R'){
					generatePawnMove(Square(i,j), turn);
				}else if (piece == 'Q'){
					generatePawnMove(Square(i,j), turn);
				}else if (piece == 'K'){
					generatePawnMove(Square(i,j), turn);
				}else 
			}
		}
		
	}
}

// Adds legal pawn moves to `moves` vector 
void Minimax::generatePawnMove(const char board[8][8], const Square &sq, const bool turn, vector<Move> &moves){
	Square target_sq(sq); //Temporary target square for move
	int dir = (turn) ? 1 : -1; //Direction pawn is moving (white moves +, black moves -)
	//Move 1 ahead
	target_sq.y = sq.y + dir;
	if (tryMove(board, Move(sq, target_sq), turn)){
		moves.push_back(Move(sq, target_sq))

		//Move 2 ahead
		if ()
		target_sq.y = sq.y + 2*dir;
		if (tryMove(board, Move(sq, target_sq), turn)){
			moves.push_back(Move(sq, target_sq))
		}
	}
	//Attack x - 1
	target_sq.x = sq.x - 1;
	target_sq.y = sq.y + dir;
	if (tryAttack(board, Move(sq, target_sq), turn)){
		moves.push_back(Move(sq, target_sq))
	}

	//Attack x + 1
	target_sq.x = sq.x + 1;
	target_sq.y = sq.y + dir;
	if (tryAttack(board, Move(sq, target_sq), turn)){
		moves.push_back(Move(sq, target_sq))
	}
}

// Adds legal knight moves to `moves` vector 
void Minimax::generateKnightMove(const char board[8][8], const Square &sq, const bool turn, vector<Move> &moves){
	Square targets[8] = [Square(sq.x - 2, sq.y + 1), Square(sq.x - 2, sq.y - 1), Square(sq.x - 1, sq.y + 2), Square(sq.x - 1, sq.y - 2), Square(sq.x + 2, sq.y + 1), Square(sq.x + 2, sq.y - 1), Square(sq.x + 1, sq.y + 2), Square(sq.x + 1, sq.y - 2)]
	for (int i = 0;i<8;i++){
		if(tryMove(board, Move(sq, targets[i]), turn)){
			moves.push_back(Move(sq, targets[i]))
		}else if(tryAttack(board, Move(sq, targets[i]), turn)){
			moves.push_back(Move(sq, targets[i]))
		}
	}
}

// Adds legal bishop moves to `moves` vector 
void Minimax::generateBishopMove(const char board[8][8], const Square &sq, const bool turn, vector<Move> &moves){
	Square dirs[4] = [Square(1, 1), Square(-1, 1), Square(1, -1), Square(-1,-1)]
	Square target;
	for (int i = 0;i<4;i++){
		target = sq;
		while (true){
			
		}
		if(tryMove(board, Move(sq, targets[i]), turn)){
			moves.push_back(Move(sq, targets[i]))
		}else if(tryAttack(board, Move(sq, targets[i]), turn)){
			moves.push_back(Move(sq, targets[i]))
		}
	}
}

bool Minimax::inCheck(const board[8][8], const bool turn){

}
