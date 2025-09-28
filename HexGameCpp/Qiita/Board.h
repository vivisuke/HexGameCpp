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
    FLAG_TERMINAL,		//	�m��]���l�i��ǂ݂ɂ��X�V�s�v�j
    FLAG_EXACT,			// �]���l�͐��m
    FLAG_LOWER,			// �]���l�͉��E
    FLAG_UPPER,			// �]���l�͏�E
};
struct TTEntry {		//	�u���\�Ɋi�[����f�[�^�\��
	float	m_score = 0.0;			//	���̋ǖʂ̕]���l
	uchar	m_flag = FLAG_UNKNOWN;
	uchar	m_depth = 0;
	ushort	m_best_move = 0;		// ���̋ǖʂł̍őP��
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
	void	print_tt(Color);				//	�u���\�̍őP��\��
	Color	next_color() const;
	void	get_tt_best_moves(Color, std::vector<int>&);
	Color	get_color(int x, int y) { return m_cell[xyToIX(x, y)]; }
	void	set_color(int x, int y, Color col) { m_cell[xyToIX(x, y)] = col; }
	void	set_color(int ix, Color col) { m_cell[ix] = col; }
	void	set_last_put_ix(int ix) { m_last_put_ix = ix; }
	void	clear_nodeSearched() const { m_nodesSearched = 0; }
	long long get_nodeSearched() const { return m_nodesSearched; }
	int		get_tt_size() const { return m_tt.size(); }
	int		n_empty() const;
	void	swap_black_white();
	int		swap_bw_ix(int ix) const;

	bool	is_winning_move(int ix, Color col, int n_empty);
	bool	is_winning_move_FO(int ix, Color col, int n_empty);		//	�Œ菇���t��
	bool	is_winning_move_always_check(int ix, Color col);	//	1�育�Ƃɏ��s�`�F�b�N
	bool	is_winning_move_check_dist(int ix, Color col);		//	1�育�Ƃɋ����`�F�b�N
	void	build_zobrist_table() const;
	void	build_fixed_order();
	int		calc_vert_dist(bool bridge = false, bool rev = false) const { return calc_dist(true, bridge, rev); }
	int		calc_horz_dist(bool bridge = false, bool rev = false) const { return calc_dist(false, bridge, rev); }
	bool	union_find(int ix, Color col);
	bool	is_vert_connected() const;		//	�㉺�ӂ��A�����Ă��邩�H
	bool	is_horz_connected() const;		//	���E�ӂ��A�����Ă��邩�H
	bool	is_vert_connected_v() const;		//	�㉺�ӂ����z�A�����Ă��邩�H
	bool	is_horz_connected_v() const;		//	���E�ӂ����z�A�����Ă��邩�H
	void	random_playout(Color next);
	void	local_playout(Color next, int ix = 0);
	bool	local_playout_to_full(Color next);		//	�󗓂������Ȃ�܂Ńv���C�A�E�g���Anext �����������ǂ�����Ԃ�
	bool	playout_to_full(Color next);		//	�󗓂������Ȃ�܂Ńv���C�A�E�g���Anext �����������ǂ�����Ԃ�
	bool	playout_to_win(Color next);		//	���s�����܂�܂Ńv���C�A�E�g���Anext �����������ǂ�����Ԃ�

	float	eval(Color next);			//	next: ��ԁA��Ԃ��猩���]���l���v�Z
	float	nega_max(Color next, int depth);		//	�Ֆʔ������]���Ȃ� nega_max
	float	nega_alpha(Color next, int depth, float alpha, float beta);		//	�Ֆʔ������]���Ȃ� nega_alpha
	float	nega_alpha_tt(Color next, int depth, float alpha, float beta, bool=false);	//	�Ֆʔ������]���Ȃ� nega_alpha�A�u���\�g�p��
	void	do_DFS(Color next, int depth);			//	depth == 0 �ɂȂ�܂Ő[���D��T��
	int		do_itrdeep(Color next, int limit);		//	limit: �T�����ԁi�~���b�P�ʁj, return: �ő�T���[��

	int		sel_move_random() const;
	int		sel_move_PMC(Color next, int limit=1000) const;	//	limit: �v�l���� �P�ʁF�~���b
	int		sel_move_local_MC(Color next, int last_ix, int last2_ix, int limit=1000) const;	//	limit: �v�l���� �P�ʁF�~���b
	int		sel_move_itrdeep(Color next, int limit=1000) const;		//	�����[���ɂ�钅��I���Alimit: �~���b�P��
private:
	void	build_fixed_order_sub(int ix);
	void	print_tt_sub(Color);				//	�u���\�̍őP��\��
	void	get_tt_best_moves_sub(Color, std::vector<int>&);
	void	get_empty_indexes(std::vector<int>&) const;
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
	void	DFS_recursive(Color next, int depth);		//	depth == 0 �ɂȂ�܂Ő[���D��T��
	void	itrdeep_recursive(Color next, int depth);		//	depth == 0 �ɂȂ�܂Ő[���D��T��
	// --- ���ԊǗ��p�̃����o�ϐ� ---
	mutable std::chrono::high_resolution_clock::time_point m_startTime;
	mutable int m_timeLimit = 0;
	mutable bool m_timeOver = false;
	mutable long long m_nodesSearched = 0; // �p�t�H�[�}���X�v���p�̒T���m�[�h��

private:
	const int	m_bd_width;
	const int	m_ary_width;
	const int	m_ary_height;
	const int	m_ary_size;
	int		m_last_put_ix = 0;
	uint64	m_hash_val = 0;				//	�Ֆʃn�b�V���l
	std::vector<Color>	m_cell;					//	�e�Z����ԁi��E���E���j
	std::vector<short>	m_parent_ul;			//	�㍶�ӕ����̐e�Z���C���f�b�N�X�z��
	std::vector<short>	m_parent_dr;			//	���E�ӕ����̐e�Z���C���f�b�N�X�z��
	std::vector<short>	m_fixed_order;			//	�Œ菇���t���Z���C���f�b�N�X�z��
	std::unordered_map<uint64, TTEntry>	m_tt;	//	�u���\�iTransposition Table�j
	mutable std::vector<uint64>	m_zobrist_black;		//	���pXOR���]�l�e�[�u��
	mutable std::vector<uint64>	m_zobrist_white;		//	���pXOR���]�l�e�[�u��
	mutable std::vector<int>	m_dist;
	mutable std::vector<byte>	m_connected;
};

