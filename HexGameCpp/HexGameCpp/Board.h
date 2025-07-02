#pragma once

#include <vector>

using byte = unsigned char;
using ushort = unsigned short;

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL,
	BYTE_MAX = 0xff,
	USHORT_MAX = 0xffff,
};

class Board
{
public:
	Board(int bd_width);
public:
	void	init();
	void	print() const;
	void	print_dist() const;
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	byte	get_color(int x, int y) const { return m_cell[xyToIndex(x, y)]; }
	void	set_color(int x, int y, byte col) { m_cell[xyToIndex(x, y)] = col; }

	int		calc_vert_dist();	//	（黒）上下辺間距離計算
	int		calc_horz_dist();	//	（白）左右辺間距離計算
	void	calc_dist_sub(int ix, int dix, ushort dist, byte col);
	void	calc_dist_sub2(int ix, int ix2, int ix3, int dix, ushort dist, byte col);

	int		eval();				//	黒から見た評価値を計算
	int		alpha_beta_black(int alpha, int beta, int depth);
	int		alpha_beta_white(int alpha, int beta, int depth);
	int		black_turn(int depth);
	int		white_turn(int depth);

public:
	int		m_bd_width;
	int		m_bd_height;
	int		m_ary_width;
	int		m_ary_height;
	int		m_ary_size;
	std::vector<byte>	m_cell;
	std::vector<ushort>	m_dist;
	std::vector<byte>	m_front;
	std::vector<byte>	m_list1;
	std::vector<byte>	m_list2;
};

