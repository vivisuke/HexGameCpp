#pragma once

#include <vector>

using byte = unsigned char;
using ushort = unsigned short;

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL,
	BYTE_MAX = 0xff,
	USHORT_MAX = 0xffff,
	DIST_MAX = 9999,
};

class Board
{
public:
	Board(int bd_width);
	Board(const Board &);
public:
	void	init();
	void	print() const;
	void	print_dist() const;
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	byte	get_color(int x, int y) const { return m_cell[xyToIndex(x, y)]; }
	byte	get_color(int ix) const { return m_cell[ix]; }
	void	set_color(int x, int y, byte col) { m_cell[xyToIndex(x, y)] = col; }
	void	set_color(int ix, byte col) { m_cell[ix] = col; }

	bool	is_vert_connected_sub(int ix);
	bool	is_vert_connected();			//	上下辺が連結されているか？ 空欄が無い状態でコールされる
	int		calc_vert_dist(bool ex=true);	//	（黒）上下辺間距離計算（６連結＋間接連結(ex)）
	int		calc_horz_dist(bool ex=true);	//	（白）左右辺間距離計算（６連結＋間接連結(ex)）
	//int		calc_vert_dist_ex();	//	（黒）上下辺間距離計算（６連結＋間接連結）
	//int		calc_horz_dist_ex();	//	（白）左右辺間距離計算（６連結＋間接連結）
	void	calc_dist_sub(int ix, int dix, ushort dist, byte col);
	void	calc_dist_sub2(int ix, int ix2, int ix3, int dix, ushort dist, byte col);

	int		eval();				//	黒から見た評価値を計算
	int		alpha_beta_black(int alpha, int beta, int depth);
	int		alpha_beta_white(int alpha, int beta, int depth);
	int		black_turn(int depth);
	int		white_turn(int depth);

	bool	playout(byte next) const;	//	完全ランダムプレイアウト、return: 黒勝ち
	bool	playout_old(byte next) const;	//	完全ランダムプレイアウト、return: 黒勝ち
	bool	playout_rave(byte next) const;	//	完全ランダムプレイアウト、return: 黒勝ち
	double	estimate_win_rate_PMC(byte next, int N) const;	//	完全ランダムプレイアウトで次手番勝率を求める
	int		sel_move_random();			//	完全ランダムに着手を選択、return: 着手箇所
	int		sel_move_PMC(byte next);			//	純粋モンテカルロ法で着手を選択、return: 着手箇所

public:
	int		m_bd_width;
	int		m_bd_height;
	int		m_ary_width;
	int		m_ary_height;
	int		m_ary_size;
	std::vector<byte>	m_cell;			//	周囲に壁（番人）を配した１次元盤面配列
	std::vector<ushort>	m_dist;
	std::vector<byte>	m_front;
	std::vector<byte>	m_list1;
	std::vector<byte>	m_list2;
	mutable std::vector<short>	m_rave;			//	統計的な各位置に打つ価値（黒白共有）
};

