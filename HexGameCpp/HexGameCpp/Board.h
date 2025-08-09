#pragma once

#include <vector>

using byte = unsigned char;
using ushort = unsigned short;

#define		is_empty()	empty()

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL,
	BYTE_MAX = 0xff,
	USHORT_MAX = 0xffff,
	DIST_MAX = 9999,
	UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8,
	UNCONNECT = -1, UL_INDEX = 0,
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
	void	print_parent() const;
	int		bd_width() const { return m_bd_width; }
	int		bd_height() const { return m_bd_height; }
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	int		indexToX(int ix) const { return ix % m_ary_width; }
	int		indexToY(int ix) const { return (ix / m_ary_width) - 1; }
	byte	get_color(int x, int y) const { return m_cell[xyToIndex(x, y)]; }
	byte	get_color(int ix) const { return m_cell[ix]; }
	void	set_color(int x, int y, byte col) { m_cell[xyToIndex(x, y)] = col; }
	void	set_color(int ix, byte col) { m_cell[ix] = col; }
	void	set_empty(int x, int y) { set_color(x, y, EMPTY); }
	bool	put_color(int x, int y, byte col);		//	return: 着手により上下 or 左右辺が連結された
	bool	put_and_check(int x, int y, byte col);
	void	check_connected(int ix, int ix2, byte col);
	bool	put_and_check_uf(int ix, byte col);
	void	check_connected_uf(int ix, int ix2, byte col);
	int		find_root(int gid);
	int		find_root_ul(int ix);
	int		find_root_dr(int ix);

	bool	is_vert_connected();			//	上下辺が連結されているか？ 空欄が無い状態でコールされる
	int		calc_vert_dist(bool ex=true);	//	（黒）上下辺間距離計算（６連結＋間接連結(ex)）
	int		calc_horz_dist(bool ex=true);	//	（白）左右辺間距離計算（６連結＋間接連結(ex)）
	//int		calc_vert_dist_ex();	//	（黒）上下辺間距離計算（６連結＋間接連結）
	//int		calc_horz_dist_ex();	//	（白）左右辺間距離計算（６連結＋間接連結）
	int		find_winning_move_black();
	int		find_winning_move_white();
	void	get_empty_list(std::vector<int>&) const;

	int		eval();				//	黒から見た評価値を計算
	int		alpha_beta_black(int alpha, int beta, int depth);
	int		alpha_beta_white(int alpha, int beta, int depth);
	int		black_turn(int depth);
	int		white_turn(int depth);

	void	playout_to_end(byte next);	//	完全ランダムプレイアウト
	bool	playout(byte next) const;	//	完全ランダムプレイアウト、return: 黒勝ち
	byte	playout_smart(byte next);	//	完全ランダムプレイアウト、return: BLACK | WHITE、勝敗判定を差分計算
	double	playout_smart(int N, byte next) const;	//	完全ランダムプレイアウト、return: 黒勝率
	bool	playout_old(byte next) const;	//	完全ランダムプレイアウト、return: 黒勝ち
	bool	playout_rave(byte next) const;	//	完全ランダムプレイアウト、return: 黒勝ち
	double	estimate_win_rate_PMC(byte next, int N) const;	//	完全ランダムプレイアウトで次手番勝率を求める
	bool	did_black_win(int ix);		//	黒 ix に打って、上下辺が連結されたか？
	int		sel_move_random();			//	完全ランダムに着手を選択、return: 着手箇所
	int		sel_move_PMC(byte next);		//	純粋モンテカルロ法で着手を選択、return: 着手箇所
	int		sel_move_MCTS(byte next);		//	モンテカルロ木探索で着手を選択、return: 着手箇所
	int		sel_move_win(byte next);		//	1手で勝ちが確定する手があればそれを返す、無ければ -1 を返す
	int		sel_move_block(byte next);		//	相手の勝利手をブロックする手があればそれを返す、無ければ -1 を返す
private:
	bool	is_vert_connected_sub(int ix);
	void	calc_dist_sub(int ix, int dix, ushort dist, byte col);
	void	calc_dist_sub2(int ix, int ix2, int ix3, int dix, ushort dist, byte col);

public:
	int		m_bd_width;					//	盤面幅
	int		m_bd_height;				//	盤面高（通常は m_bd_width と同値）
	int		m_ary_width;				//	周囲の壁（番人）を含む盤面幅
	int		m_ary_height;				//	周囲の壁（番人）を含む盤面高
	int		m_ary_size;					//	１次元盤面配列サイズ
	int		DR_INDEX;
	short	m_next_gid;
	std::vector<byte>	m_cell;			//	周囲に壁（番人）を配した１次元盤面配列
	std::vector<short>	m_gid;			//	各石のグループID
	std::vector<byte>	m_min_gid;		//	接続しているグループの最小グループID
	std::vector<byte>	m_gid_connected;	//	各グループIDの石が上下左右辺に接続しているか？
	std::vector<ushort>	m_dist;			//	上下・左右距離計測用
	std::vector<byte>	m_front;		//	上下・左右距離計測：次探索位置
	std::vector<byte>	m_list1;
	std::vector<byte>	m_list2;
	std::vector<short>	m_parent_ul;			//	上左辺方向の親セルインデックス配列
	std::vector<short>	m_parent_dr;			//	下右辺方向の親セルインデックス配列
	mutable std::vector<short>	m_rave;			//	統計的な各位置に打つ価値（黒白共有）
};

bool dfs_black_win(Board&, byte next);		//	双方最善で黒が勝つか？

