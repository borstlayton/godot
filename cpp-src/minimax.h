#ifndef MINIMAX_H
#define MINIMAX_H

// #include <godot_cpp/classes/sprite2d.hpp>
#include <vector>
#include <unordered_map>
#include <random> 

// namespace godot {

// class Minimax : public Sprite2D {
// 	GDCLASS(Minimax, Sprite2D)
class Minimax{
private:
	unsigned long long int ZobristTable[8][8][12];
	mt19937 mt;
	unordered_map<char, float> piece_value;

protected:
	// static void _bind_methods();
	Square findKing(const char board[8][8], const bool &turn);
	bool inCheck(const char board[8][8], const bool &turn);
	void generateKingAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks);
	void generateKingMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves);
	void generateQueenAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks);
	void generateQueenMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves);
	void generateRookAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks);
	void generateRookMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves);
	void generateBishopAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks);
	void generateBishopMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves);
	void generateKnightAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks);
	void generateKnightMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves);
	void generatePawnAttack(char board[8][8], const Square &sq, const bool &turn, vector<Move> attacks);
	void generatePawnMove(char board[8][8], const Square &sq, const bool &turn, vector<Move> moves);
	vector<Move> generateAttacks(char board[8][8], const bool turn);
	vector<Move> generateMoves(char board[8][8], const bool turn);
	bool isLower(const char &letter);
	bool isUpper(const char &letter);
	bool isValidMove(char board[8][8], const Move &mv, const bool &turn);
	bool tryAddAttack(char board[8][8], const Move &mv, const bool &turn, vector<Move> attacks);
	bool tryAddMove(char board[8][8], const Move &mv, const bool &turn, vector<Move> moves);
	unsigned long long int computeHash(const char board[8][8]);
	void initTable();
	unsigned long long int randomInt();
	int indexOf(char piece);
	float eval(const char board[8][8]);
	float abMinimaxAttacks(char board[8][8], bool turn, float alpha, float beta, std::unordered_map<unsigned long long int, float> zobrist);
	float abMinimax(char board[8][8], int depth, bool turn, float alpha, float beta, std::unordered_map<unsigned long long int, float> zobrist);



public:
	Minimax();
	~Minimax();

	// void _process(double delta) override;
	Move findBest(char board[8][8], bool turn, unsigned int depth);
};
// }

class Square{
public:
	int x;
	int y;
	Square();
	Square(const int &p_x, const int &p_y);
	Square operator+(const Square &p_sq);
	Square operator=(const Square &p_sq);
};

class Move{
public:
	Square sq1;
	Square sq2;
	char promotion;
	Move();
	Move(const Square &p_sq1, const Square &p_sq2, char p_promotion);
	Move operator=(const Move &p_mv);
};

#endif