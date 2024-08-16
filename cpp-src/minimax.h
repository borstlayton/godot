#ifndef MINIMAX_H
#define MINIMAX_H

#include <godot_cpp/classes/sprite2d.hpp>

namespace godot {

class Minimax : public Sprite2D {
	GDCLASS(Minimax, Sprite2D)

private:
	unsigned long long int ZobristTable[8][8][12];
	std::mt19937 mt(01234567);
	unordered_map<char, float> piece_value;

protected:
	static void _bind_methods();

public:
	Minimax();
	~Minimax();
    void set_amplitude(const double p_amplitude);
	double get_amplitude() const;

	void _process(double delta) override;
    void set_speed(const double p_speed);
	double get_speed() const;
};
}

class Square{
public:
	int x;
	int y;
	Square();
	Square(const int p_x, const int p_y);
	Square operator=(const Square &p_sq);
};

class Move{
	Square sq1;
	Square sq2;
	char promotion;
	Move();
	Move(const Square &p_sq1, const Square &p_sq2);
	Move operator=(const Move &p_mv);
};

#endif