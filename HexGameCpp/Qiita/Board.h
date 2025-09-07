#pragma once

#include <vector>
#include <chrono>

using Color = unsigned char;
using byte = unsigned char;

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
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	int		ixToX(int ix) const { return ix % m_ary_width; }
	int		ixToY(int ix) const { return (ix / m_ary_width) - 1; }
	void	print() const;
	void	set_color(int x, int y, Color col) { m_cell[xyToIndex(x, y)] = col; }
	void	set_color(int ix, Color col) { m_cell[ix] = col; }
	void	get_empty_indexes(std::vector<int>&) const;
	void	set_last_put_ix(int ix) { m_last_put_ix = ix; }

	bool	union_find(int ix, Color col);
	bool	is_vert_connected() const;		//	上下辺が連結しているか？
	bool	is_horz_connected() const;		//	左右辺が連結しているか？
	void	random_playout(Color next);
	bool	playout_to_full(Color next);		//	空欄が無くなるまでプレイアウトし、next が勝ったかどうかを返す
	bool	playout_to_win(Color next);		//	勝敗が決まるまでプレイアウトし、next が勝ったかどうかを返す

	int		sel_move_random() const;
	int		sel_move_PMC(Color next, int limit=1000) const;	//	limit: 思考時間 単位：ミリ秒
private:
	bool	is_vert_connected_DFS(int ix) const;
	bool	is_horz_connected_DFS(int ix) const;
	//bool	is_connected_DFS_sub(int ix) const {
	//	return m_cell[ix] == BLACK && m_connected[ix] == UNSEARCHED &&
	//			is_vert_connected_DFS(ix);
	//}
	int		find_root_ul(int ix);
	int		find_root_dr(int ix);
	void	check_connected_uf(int ix, int ix2, Color col);
	// --- 時間管理用のメンバ変数 ---
	mutable std::chrono::high_resolution_clock::time_point m_startTime;
	mutable int m_timeLimit = 0;
	mutable bool m_timeOver = false;
	mutable long long m_nodesSearched = 0; // パフォーマンス計測用の探索ノード数

private:
	const int	m_bd_width;
	const int	m_ary_width;
	const int	m_ary_height;
	const int	m_ary_size;
	//const int	UL_INDEX = 0;					//	上左仮想ルート for Union-Find
	//int			DR_INDEX;						//	下右辺仮想ルート for Union-Find
	int		m_last_put_ix = 0;
	std::vector<Color>	m_cell;					//	各セル状態（空・黒・白）
	std::vector<short>	m_parent_ul;			//	上左辺方向の親セルインデックス配列
	std::vector<short>	m_parent_dr;			//	下右辺方向の親セルインデックス配列
	mutable std::vector<byte>	m_connected;
};

