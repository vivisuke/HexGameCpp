#pragma once

#include <vector>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <chrono>


using Color = unsigned char;
using byte = unsigned char;
using uchar = unsigned char;
using ushort = unsigned short;
using uint64 = uint64_t;

enum {
	EMPTY = 0, BLACK, WHITE, WALL,
	UNSEARCHED = 0, SEARCHED,
	UNCONNECT = -1,
	TL_INDEX = 0, BT_INDEX, RT_INDEX,
};

class Board
{
public:
	Board(int width);
	Board& operator=(const Board&);
public:
	void	init();
	int		get_width() const { return m_bd_width; }
	int		xyToIX(int x, int y) const { return (y+1)*m_ary_width + x; }
	int		ixToX(int ix) const { return ix % m_ary_width; }
	int		ixToY(int ix) const { return (ix / m_ary_width) - 1; }
	std::string ixToStr(int ix) const;
	void	print() const;
	void	print_parent_ul() const;
	Color	next_color() const;
	int		n_empty() const;
	void	get_empty_indexes(std::vector<int>&) const;
	Color	get_color(int x, int y) { return m_cell[xyToIX(x, y)]; }
	void	set_color(int x, int y, Color col) { m_cell[xyToIX(x, y)] = col; }
	void	set_color(int ix, Color col) { m_cell[ix] = col; }
	void	set_color_uf(int x, int y, Color col) { set_color_uf(xyToIX(x, y), col); }
	void	set_color_uf(int ix, Color col) {
				m_cell[ix] = col;
				union_find(ix, col);
			}
	void	set_last_put_ix(int ix) { m_last_put_ix = ix; }

	bool	is_vert_connected() const;		//	上下辺が連結しているか？
	bool	is_horz_connected() const;		//	左右辺が連結しているか？
	bool	is_vert_connected_v() const;		//	上下辺が仮想連結しているか？
	bool	is_horz_connected_v() const;		//	左右辺が仮想連結しているか？
	bool	union_find(int ix, Color col);
	void	undo_union_find();
	bool	union_find_v(int ix, Color col);	//	ブリッジ対応
private:
	bool	is_vert_connected_DFS(int ix) const;
	bool	is_horz_connected_DFS(int ix) const;
	bool	is_vert_connected_v_DFS(int ix, int mx1=-1, int mx2=-1) const;
	bool	is_horz_connected_v_DFS(int ix, int mx1=-1, int mx2=-1) const;
	void	check_connected_uf(int ix, int ix2, Color col);
	void	check_connected_uf_v(int ix, int ix2, Color col);
	int		find_root_ul(int ix);
private:
	const int	m_bd_width;
	const int	m_ary_width;
	const int	m_ary_height;
	const int	m_ary_size;
	int		m_last_put_ix = 0;

	std::vector<Color>	m_cell;					//	各セル状態（空・黒・白）
	std::vector<short>	m_parent_ul;			//	上左辺方向の親セルインデックス配列
	//std::vector<short>	m_parent_ul_v;			//	上左辺方向の親セルインデックス配列（ブリッジ対応）
	//std::vector<short>	m_parent_dr;			//	下右辺方向の親セルインデックス配列
	std::vector<short>	m_uf_stack;				//	Unidon-Find 用スタック for undo
	mutable std::vector<byte>	m_connected;
};

