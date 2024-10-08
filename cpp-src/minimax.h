#ifndef MINIMAX_H
#define MINIMAX_H

// #include <godot_cpp/classes/sprite2d.hpp>
#include <vector>
#include <unordered_map>
#include <random> 

// namespace godot {

// class Minimax : public Sprite2D {
// 	GDCLASS(Minimax, Sprite2D)

class Square{
public:
	int x;
	int y;
	Square();
	Square(const int &p_x, const int &p_y);
	Square operator+(const Square &p_sq) const;
	Square operator=(const Square &p_sq);
	bool operator==(const Square &p_sq) const;
};

class Move{
public:
	Square sq1;
	Square sq2;
	char promotion;
	Move();
	Move(const Square &p_sq1, const Square &p_sq2, char p_promotion);
	Move(const char *move);
	Move operator=(const Move &p_mv);
	void printMove() const;
	Move stringToMove(const char *move);
};

class Minimax{
private:
	unsigned long long int ZobristTable[8][8][12];
	std::mt19937 mt;
	std::unordered_map<char, float> piece_value;

// protected:
public:
	unsigned int num_boards;
	unsigned int hashes_found;
	// static void _bind_methods();
	void printBoard(const char board[8][8]);
	Square findKing(const char board[8][8], const bool turn);
	bool inCheck(const char board[8][8], const bool turn);
	void generateKingAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks);
	void generateKingMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves);
	void generateQueenAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks);
	void generateQueenMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves);
	void generateRookAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks);
	void generateRookMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves);
	void generateBishopAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks);
	void generateBishopMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves);
	void generateKnightAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks);
	void generateKnightMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves);
	void generatePawnAttack(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &attacks);
	void generatePawnMove(char board[8][8], const Square &sq, const bool turn, std::vector<Move> &moves);
	std::vector<Move> generateAttacks(char board[8][8], const bool turn);
	std::vector<Move> generateMoves(char board[8][8], const bool turn);
	bool isLower(const char letter);
	bool isUpper(const char letter);
	bool isValidMove(char board[8][8], const Move &mv, const bool turn);
	bool tryAddAttack(char board[8][8], const Move &mv, const bool turn, std::vector<Move> &attacks);
	bool tryAddMove(char board[8][8], const Move &mv, const bool turn, std::vector<Move> &moves);
	unsigned long long int computeHash(const char board[8][8]);
	void initTable();
	unsigned long long int randomInt();
	int indexOf(const char piece);
	float eval(const char board[8][8]);
	float abMinimaxAttacks(char board[8][8], bool turn, float alpha, float beta, std::unordered_map<unsigned long long int, float> &zobrist);
	float abMinimax(char board[8][8], unsigned int depth, bool turn, float alpha, float beta, std::unordered_map<unsigned long long int, float> &zobrist);

// public:
	Minimax();
	~Minimax();

	// void _process(double delta) override;
	Move findBest(char board[8][8], bool turn, unsigned int depth);
};
// }

#endif