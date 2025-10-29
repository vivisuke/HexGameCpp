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
enum TTFlag {
    FLAG_UNKNOWN,
    FLAG_TERMINAL,		//	確定評価値（先読みによる更新不要）
    FLAG_EXACT,			// 評価値は正確
    FLAG_LOWER,			// 評価値は下界
    FLAG_UPPER,			// 評価値は上界
};
struct TT2Entry {		//	置換表に格納するデータ構造 for 勝敗のみ完全解析
	uchar	m_flag = FLAG_UNKNOWN;
	bool	m_winning;					//	次の手番の方の勝ち
};
inline Color oppo_color(Color col) { return BLACK + WHITE - col; }

class Board
{
public:
	Board(int width);
	Board& operator=(const Board&);
public:
	void	init();
	void	build_fixed_order();
	void	build_zobrist_table() const;						//	ハッシュ用テーブル構築
	int		get_width() const { return m_bd_width; }
	size_t	get_tt2_size() const { return m_tt2.size(); }
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
	int		calc_vert_dist(bool bridge = false, bool rev = false) const { return calc_dist(true, bridge, rev); }
	int		calc_horz_dist(bool bridge = false, bool rev = false) const { return calc_dist(false, bridge, rev); }

	bool	is_winning_move(int ix, Color col);		//	固定順序付け、一手ごとに見合い連結チェック
	bool	is_winning_move_dist1(int ix, Color col, bool=true);		//	固定順序付け、距離１なら勝ち判定
	bool	is_winning_move_TT(int ix, Color col);		//	固定順序付け、一手ごとに見合い連結チェック、置換表
	bool	is_winning_move_dist1_TT(int ix, Color col);		//	固定順序付け、一手ごとに見合い連結チェック、置換表
private:
	void	build_fixed_order_sub(int ix, int len);
	bool	is_vert_connected_DFS(int ix) const;
	bool	is_horz_connected_DFS(int ix) const;
	bool	is_vert_connected_v_DFS(int ix, int mx1=-1, int mx2=-1) const;
	bool	is_horz_connected_v_DFS(int ix, int mx1=-1, int mx2=-1) const;
	void	check_connected_uf(int ix, int ix2, Color col);
	void	check_connected_uf_v(int ix, int ix2, Color col);
	int		find_root_ul(int ix);
	int		calc_dist(bool vertical, bool bridge, bool rev) const;
	bool	is_winning_position(Color col);		//	次の手番が勝ちか？
	bool	is_winning_position_TT(Color col);		//	次の手番が勝ちか？
	bool	is_winning_position_dist1_TT(Color col, bool=true);		//	次の手番が勝ちか？
private:
	const int	m_bd_width;
	const int	m_ary_width;
	const int	m_ary_height;
	const int	m_ary_size;
	int		m_last_put_ix = 0;
	uint64	m_hash_val = 0;				//	盤面ハッシュ値
	uint64	m_hash_val2 = 0;			//	180度回転盤面ハッシュ値

	std::vector<Color>	m_cell;					//	各セル状態（空・黒・白）
	std::vector<short>	m_parent_ul;			//	上左辺方向の親セルインデックス配列
	//std::vector<short>	m_parent_ul_v;			//	上左辺方向の親セルインデックス配列（ブリッジ対応）
	//std::vector<short>	m_parent_dr;			//	下右辺方向の親セルインデックス配列
	std::vector<short>	m_fixed_order;			//	固定順序付けセルインデックス配列
	std::vector<short>	m_uf_stack;				//	Unidon-Find 用スタック for undo
	std::unordered_map<uint64, TT2Entry>	m_tt2;	//	置換表（Transposition Table）for 勝敗のみ完全解析

	mutable std::vector<uint64>	m_zobrist_black;		//	黒用XOR反転値テーブル
	mutable std::vector<uint64>	m_zobrist_white;		//	白用XOR反転値テーブル
	mutable std::vector<int>	m_dist;
	mutable std::vector<byte>	m_connected;
};

