#pragma once

#include <vector>

using byte = unsigned char;

enum {
	EMPTY = 0, BLACK, WHITE, WALL,
	UNSEARCHED = 0, SEARCHED,
};

class Board
{
public:
	Board(int width);
	//	  : m_width(width), m_ary_width(width + 1),
	//	  m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
	//{
	//}
public:
	void	init();
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	int		ixToX(int ix) const { return ix % m_ary_width; }
	int		ixToY(int ix) const { return (ix / m_ary_width) - 1; }
	void	print() const;
	void	set_color(int x, int y, byte col) { m_cell[xyToIndex(x, y)] = col; }
	void	set_color(int ix, byte col) { m_cell[ix] = col; }
	void	get_empty_indexes(std::vector<int>&) const;

	bool	is_vert_connected() const;		//	上下辺が連結しているか？
	bool	is_horz_connected() const;		//	左右辺が連結しているか？
	void	random_playout(byte next);
	bool	playout_to_full(byte next);		//	空欄が無くなるまでプレイアウトし、next が勝ったかどうかを返す
	bool	playout_to_win(byte next);		//	勝敗が決まるまでプレイアウトし、next が勝ったかどうかを返す
private:
	bool	is_vert_connected_DFS(int ix) const;
	bool	is_horz_connected_DFS(int ix) const;
	//bool	is_connected_DFS_sub(int ix) const {
	//	return m_cell[ix] == BLACK && m_connected[ix] == UNSEARCHED &&
	//			is_vert_connected_DFS(ix);
	//}

private:
	const int	m_bd_width;
	const int	m_ary_width;
	const int	m_ary_height;
	const int	m_ary_size;
	std::vector<byte>	m_cell;
	mutable std::vector<byte>	m_connected;
};

