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
struct TTEntry {		//	置換表に格納するデータ構造
	float	m_score = 0.0;			//	この局面の評価値
	uchar	m_flag = FLAG_UNKNOWN;
	uchar	m_depth = 0;
	ushort	m_best_move = 0;		// この局面での最善手
};
struct TT2Entry {		//	置換表に格納するデータ構造 for 勝敗のみ完全解析
	uchar	m_flag = FLAG_UNKNOWN;
	bool	m_winning;					//	次の手番の方の勝ち
};
struct TT3Entry {		//	置換表に格納するデータ構造 for 勝敗のみ完全解析
	uchar	m_flag = FLAG_UNKNOWN;
	short	m_eval = 0;					//	次の手番から見た評価値、プラス：勝ち、マイナス：負け
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
	void	print_dist() const;
	void	print_tt(Color);				//	置換表の最善手表示
	Color	next_color() const;
	void	get_tt_best_moves(Color, std::vector<int>&);
	size_t	get_tt2_size() const { return m_tt2.size(); }
	size_t	get_tt3_size() const { return m_tt3.size(); }
	Color	get_color(int x, int y) { return m_cell[xyToIX(x, y)]; }
	void	set_color(int x, int y, Color col) { m_cell[xyToIX(x, y)] = col; }
	void	set_color(int ix, Color col) { m_cell[ix] = col; }
	void	set_last_put_ix(int ix) { m_last_put_ix = ix; }
	void	clear_nodeSearched() const { m_nodesSearched = 0; }
	long long get_nodeSearched() const { return m_nodesSearched; }
	void	get_empty_indexes(std::vector<int>&) const;
	int		get_tt_size() const { return m_tt.size(); }
	int		n_empty() const;
	void	swap_black_white();
	int		swap_bw_ix(int ix) const;

	bool	is_winning_move(int ix, Color col, short n_empty);		//	空欄無しまで着手→勝敗判定
	bool	is_winning_move_FO(int ix, Color col, short n_empty);		//	固定順序付け
	bool	is_winning_move_always_check(int ix, Color col);	//	1手ごとに勝敗チェック
	bool	is_winning_move_check_dist(int ix, Color col);		//	1手ごとに距離チェック
	bool	is_winning_move_check_dist_FO(int ix, Color col);	//	1手ごとに距離チェック
	bool	is_winning_move_TT(int ix, Color col);				//	置換表使用
	void	build_zobrist_table() const;
	void	build_fixed_order();
	int		calc_vert_dist(bool bridge = false, bool rev = false) const { return calc_dist(true, bridge, rev); }
	int		calc_horz_dist(bool bridge = false, bool rev = false) const { return calc_dist(false, bridge, rev); }
	bool	union_find(int ix, Color col);
	bool	is_vert_connected() const;		//	上下辺が連結しているか？
	bool	is_horz_connected() const;		//	左右辺が連結しているか？
	bool	is_vert_connected_v() const;		//	上下辺が仮想連結しているか？
	bool	is_horz_connected_v() const;		//	左右辺が仮想連結しているか？
	bool	is_vert_connected_BFS() const;
	bool	is_horz_connected_BFS() const;
	bool	is_vert_connected_v_BFS() const;
	bool	is_horz_connected_v_BFS() const;
	void	random_playout(Color next);
	void	local_playout(Color next, int ix = 0);
	bool	local_playout_to_full(Color next);		//	空欄が無くなるまでプレイアウトし、next が勝ったかどうかを返す
	bool	playout_to_full(Color next);		//	空欄が無くなるまでプレイアウトし、next が勝ったかどうかを返す
	bool	playout_to_win(Color next);			//	勝敗が決まるまでプレイアウトし、next が勝ったかどうかを返す

	short	nh_nega_alpha_TT(int ix, Color next, short n_empty);
	short	nega_max_FO(int ix, Color next, short n_empty);	//	1手ごとに（６近傍＋仮想連結）距離チェック＋固定順序付け
	short	nega_alpha_FO(int ix, Color next, short n_empty);
	short	nega_alpha_TT(int ix, Color next, short n_empty);


	float	eval(Color next);			//	next: 手番、手番から見た評価値を計算
	float	nega_max(Color next, int depth);		//	盤面白黒反転しない nega_max
	float	nega_alpha(Color next, int depth, float alpha, float beta);		//	盤面白黒反転しない nega_alpha
	float	nega_alpha_tt(Color next, int depth, float alpha, float beta, bool=false);	//	盤面白黒反転しない nega_alpha、置換表使用版
	void	do_DFS(Color next, int depth);			//	depth == 0 になるまで深さ優先探索
	int		do_itrdeep(Color next, int limit);		//	limit: 探索時間（ミリ秒単位）, return: 最大探索深さ

	int		sel_move_random() const;
	int		sel_move_PMC(Color next, int limit=1000) const;	//	limit: 思考時間 単位：ミリ秒
	int		sel_move_local_MC(Color next, int last_ix, int last2_ix, int limit=1000) const;	//	limit: 思考時間 単位：ミリ秒
	int		sel_move_itrdeep(Color next, int limit=1000) const;		//	反復深化による着手選択、limit: ミリ秒単位
private:
	void	build_fixed_order_sub(int ix, int len);
	void	print_tt_sub(Color);				//	置換表の最善手表示
	void	get_tt_best_moves_sub(Color, std::vector<int>&);
	void	get_subdiagonal_indexes(std::vector<int>&) const;
	void	get_local_indexes(std::vector<int>&, int last_ix = 0, int last2_ix = 0) const;
	void	add_bridge_indexes(std::vector<int>&, int last_ix) const;
	bool	is_vert_connected_DFS(int ix) const;
	bool	is_horz_connected_DFS(int ix) const;
	bool	is_vert_connected_v_DFS(int ix) const;
	bool	is_horz_connected_v_DFS(int ix) const;
	int		calc_dist(bool vertical, bool bridge, bool rev) const;
	int		find_root_ul(int ix);
	int		find_root_dr(int ix);
	void	check_connected_uf(int ix, int ix2, Color col);
	void	DFS_recursive(Color next, int depth);		//	depth == 0 になるまで深さ優先探索
	void	itrdeep_recursive(Color next, int depth);		//	depth == 0 になるまで深さ優先探索
	short	nh_nega_alpha_TT_sub(Color next, short n_empty, short alpha, short beta);	//	1手ごとに６近傍連結チェック
	short	nega_max_FO_sub(Color next, short n_empty);	//	1手ごとに（６近傍＋仮想連結）距離チェック＋固定順序付け
	short	nega_alpha_FO_sub(Color next, short n_empty, short alpha, short beta);	//	1手ごとに（６近傍＋仮想連結）距離チェック＋固定順序付け
	short	nega_alpha_TT_sub(Color next, short n_empty, short alpha, short beta);	//	1手ごとに（６近傍＋仮想連結）距離チェック＋固定順序付け
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
	int		m_last_put_ix = 0;
	uint64	m_hash_val = 0;				//	盤面ハッシュ値
	uint64	m_hash_val2 = 0;			//	180度回転盤面ハッシュ値
	std::vector<Color>	m_cell;					//	各セル状態（空・黒・白）
	std::vector<short>	m_parent_ul;			//	上左辺方向の親セルインデックス配列
	std::vector<short>	m_parent_dr;			//	下右辺方向の親セルインデックス配列
	std::vector<short>	m_fixed_order;			//	固定順序付けセルインデックス配列
	std::vector<short>	m_rot180_table;			//	180度回転用インデックス配列
	std::unordered_map<uint64, TTEntry>	m_tt;	//	置換表（Transposition Table）
	std::unordered_map<uint64, TT2Entry>	m_tt2;	//	置換表（Transposition Table）for 勝敗のみ完全解析
	std::unordered_map<uint64, TT3Entry>	m_tt3;	//	置換表（Transposition Table）for 最短勝ち完全解析
	mutable std::vector<uint64>	m_zobrist_black;		//	黒用XOR反転値テーブル
	mutable std::vector<uint64>	m_zobrist_white;		//	白用XOR反転値テーブル
	mutable std::vector<int>	m_dist;
	mutable std::vector<byte>	m_connected;
};

