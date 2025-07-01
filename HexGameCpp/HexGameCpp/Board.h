#pragma once

#include <vector>

using byte = unsigned char;

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL,
};

class Board
{
public:
	Board(int bd_width);
public:
	void	init();
	void	print() const;
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	byte	get_color(int x, int y) const { return m_board[xyToIndex(x, y)]; }
	void	set_color(int x, int y, byte col) { m_board[xyToIndex(x, y)] = col; }
public:
	int		m_bd_width;
	int		m_bd_height;
	int		m_ary_width;
	int		m_ary_height;
	int		m_ary_size;
	std::vector<byte>	m_board;
};

