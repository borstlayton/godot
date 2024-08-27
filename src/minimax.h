#ifndef MINIMAX_H
#define MINIMAX_H

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <vector>
#include <unordered_map>
#include <random> 

namespace godot {

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

class Chess{
private:
	unsigned long long int ZobristTable[8][8][12];
	std::mt19937 mt;
	std::unordered_map<char, float> piece_value;
	std::unordered_map<unsigned long long int, float> zobrist;
	char board[8][8];
	bool turn;
	unsigned int num_boards;
	unsigned int hashes_found;

	void generateKingAttack(const Square &sq,  std::vector<Move> &attacks);
	void generateKingMove(const Square &sq,  std::vector<Move> &moves);
	void generateQueenAttack(const Square &sq,  std::vector<Move> &attacks);
	void generateQueenMove(const Square &sq,  std::vector<Move> &moves);
	void generateRookAttack(const Square &sq,  std::vector<Move> &attacks);
	void generateRookMove(const Square &sq,  std::vector<Move> &moves);
	void generateBishopAttack(const Square &sq,  std::vector<Move> &attacks);
	void generateBishopMove(const Square &sq,  std::vector<Move> &moves);
	void generateKnightAttack(const Square &sq,  std::vector<Move> &attacks);
	void generateKnightMove(const Square &sq,  std::vector<Move> &moves);
	void generatePawnAttack(const Square &sq,  std::vector<Move> &attacks);
	void generatePawnMove(const Square &sq,  std::vector<Move> &moves);
	bool isValidMove(const Move &mv);
	bool tryAddAttack(const Move &mv,  std::vector<Move> &attacks);
	bool tryAddMove(const Move &mv,  std::vector<Move> &moves);
	
	void initTable();
	unsigned long long int randomInt() const;
	int indexOf(const char piece) const;
	

public:
	Chess();
	void setBoard(varient board);
	char at(const Square sq) const;
	char at(const unsigned int i, const unsigned int j) const;
	char set(const Square sq, const char piece);
	float eval() const;
	Square findKing() const;
	bool inCheck() const;
	std::vector<Move> generateAll();
	std::vector<Move> generateAttacks();
	std::vector<Move> generateMoves();
	bool isLower(const char letter) const;
	bool isUpper(const char letter) const;
	unsigned long long int computeHash() const;
	void printBoard() const;
}

// class Minimax{
class Minimax : public Node2D {
	GDCLASS(Minimax, Node2D)

private:
	static int DEPTH;
	float abMinimaxAttacks(Chess &Board, float alpha, float beta);
	float abMinimax(Chess &Board, unsigned int depth, float alpha, float beta);

protected:
	static void _bind_methods();

public:
	void setDepth(const int d);
	// void _process(double delta) override;
	void _ready() override;
	Move findBest(Chess &Board, bool turn, unsigned int depth);
};
}

#endif