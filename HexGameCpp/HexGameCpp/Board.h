#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

using byte = unsigned char;
using uchar = unsigned char;
using ushort = unsigned short;
using uint64 = uint64_t;

#define		is_empty()	empty()

enum {
	EMPTY = 0, BLACK, WHITE, BWALL, WWALL,
	BYTE_MAX = 0xff,
	USHORT_MAX = 0xffff,
	DIST_MAX = 9999,
	UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8,
	UNCONNECT = -1, UL_INDEX = 0,
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
	Board(int bd_width);
	Board(const Board &);
public:
	void	init();
	void	print() const;
	void	print_dist() const;
	void	print_parent() const;
	void	print_tt(byte);				//	�u���\�̍őP��\��
	void	get_tt_best_moves(byte, std::vector<int>&);
	int		bd_width() const { return m_bd_width; }
	int		bd_height() const { return m_bd_height; }
	int		xyToIndex(int x, int y) const { return (y+1)*m_ary_width + x; }
	int		indexToX(int ix) const { return ix % m_ary_width; }
	int		indexToY(int ix) const { return (ix / m_ary_width) - 1; }
	std::string ixToStr(int ix) const;
	byte	get_color(int x, int y) const { return m_cell[xyToIndex(x, y)]; }
	byte	get_color(int ix) const { return m_cell[ix]; }
	void	set_color(int x, int y, byte col) { m_cell[xyToIndex(x, y)] = col; }
	void	set_color(int ix, byte col) { m_cell[ix] = col; }
	void	set_empty(int x, int y) { set_color(x, y, EMPTY); }
	bool	put_color(int x, int y, byte col);		//	return: ����ɂ��㉺ or ���E�ӂ��A�����ꂽ
	bool	put_and_check(int x, int y, byte col);
	void	check_connected(int ix, int ix2, byte col);
	bool	put_and_check_uf(int ix, byte col);
	void	check_connected_uf(int ix, int ix2, byte col);
	int		find_root(int gid);
	int		find_root_ul(int ix);
	int		find_root_dr(int ix);

	bool	is_vert_connected();			//	�㉺�ӂ��A������Ă��邩�H �󗓂�������ԂŃR�[�������
	int		calc_vert_dist(bool ex=true, bool ud=true);	//	�i���j�㉺�ӊԋ����v�Z�i�U�A���{�ԐژA��(ex)�j
	int		calc_horz_dist(bool ex=true, bool lr=true);	//	�i���j���E�ӊԋ����v�Z�i�U�A���{�ԐژA��(ex)�j
	//int		calc_vert_dist_ex();	//	�i���j�㉺�ӊԋ����v�Z�i�U�A���{�ԐژA���j
	//int		calc_horz_dist_ex();	//	�i���j���E�ӊԋ����v�Z�i�U�A���{�ԐژA���j
	int		find_winning_move_black();
	int		find_winning_move_white();
	void	find_winning_moves_black(std::vector<int>&, bool ex=true);
	void	find_winning_moves_white(std::vector<int>&);
	void	get_empty_list(std::vector<int>&) const;
	int		n_empty() const;
	void	swap_black_white();
	int		swap_bw_ix(int ix) const;

	float	eval_black();				//	���ԁA�����猩���]���l���v�Z
	float	eval_white();				//	���ԁA�����猩���]���l���v�Z
	float	eval(byte next);			//	next: ��ԁA��Ԃ��猩���]���l���v�Z
	float	nega_max(byte next, int depth);		//	�Ֆʔ������]���Ȃ� nega_max
	float	nega_alpha(byte next, int depth, float alpha, float beta);		//	�Ֆʔ������]���Ȃ� nega_alpha
	int		alpha_beta_black(int alpha, int beta, int depth);
	int		alpha_beta_white(int alpha, int beta, int depth);
	int		black_turn(int depth);
	int		white_turn(int depth);

	void	playout_to_end(byte next);	//	���S�����_���v���C�A�E�g
	bool	playout(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	byte	playout_smart(byte next);	//	���S�����_���v���C�A�E�g�Areturn: BLACK | WHITE�A���s����������v�Z
	double	playout_smart(int N, byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	byte	get_playout_winner(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: BLACK or WHITE
	bool	playout_old(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	bool	playout_rave(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	double	estimate_win_rate_PMC(byte next, int N) const;	//	���S�����_���v���C�A�E�g�Ŏ���ԏ��������߂�
	bool	did_black_win(int ix);		//	�� ix �ɑł��āA�㉺�ӂ��A�����ꂽ���H
	int		sel_move_random();			//	���S�����_���ɒ����I���Areturn: ����ӏ�
	int		sel_move_PMC(byte next, int limit=1000);		//	���������e�J�����@�Œ����I���Alimit: �~���b�P��
	int		sel_move_MCTS(byte next);		//	�����e�J�����ؒT���Œ����I���Areturn: ����ӏ�
	int		sel_move_win(byte next);		//	1��ŏ������m�肷��肪����΂����Ԃ��A������� -1 ��Ԃ�
	int		sel_move_block(byte next);		//	����̏�������u���b�N����肪����΂����Ԃ��A������� -1 ��Ԃ�
	int		sel_move_heuristic(byte next);			//	
	int		sel_move_AB(byte next);			//	�����@�{�]���֐��ɂ�钅��I��
	int		sel_move_itrdeep(byte next, int limit=1000);		//	�����[���ɂ�钅��I���Alimit: �~���b�P��
private:
	void	print_tt_sub(byte);				//	�u���\�̍őP��\��
	void	get_tt_best_moves_sub(byte, std::vector<int>&);
	bool	is_vert_connected_sub(int ix);
	void	calc_dist_sub(int ix, int dix, ushort dist, byte col);
	void	calc_dist_sub2(int ix, int ix2, int ix3, int dix, ushort dist, byte col);
	void	build_zobrist_table();
	float	nega_max_tt(byte next, int depth);		//	�Ֆʔ������]���Ȃ� nega_max�A�u���\�g�p��
	float	nega_alpha_tt(byte next, int depth, float alpha, float beta);	//	�Ֆʔ������]���Ȃ� nega_alpha�A�u���\�g�p��

public:
	int		m_bd_width;					//	�Ֆʕ�
	int		m_bd_height;				//	�Ֆʍ��i�ʏ�� m_bd_width �Ɠ��l�j
	int		m_ary_width;				//	���͂̕ǁi�Ԑl�j���܂ޔՖʕ�
	int		m_ary_height;				//	���͂̕ǁi�Ԑl�j���܂ޔՖʍ�
	int		m_ary_size;					//	�P�����Ֆʔz��T�C�Y
	int		DR_INDEX;
	short	m_next_gid;
	uint64	m_hash_val;				//	�Ֆʃn�b�V���l
	std::vector<byte>	m_cell;			//	���͂ɕǁi�Ԑl�j��z�����P�����Ֆʔz��
	std::vector<short>	m_gid;			//	�e�΂̃O���[�vID
	std::vector<byte>	m_min_gid;		//	�ڑ����Ă���O���[�v�̍ŏ��O���[�vID
	std::vector<byte>	m_gid_connected;	//	�e�O���[�vID�̐΂��㉺���E�ӂɐڑ����Ă��邩�H
	std::vector<ushort>	m_dist;			//	�㉺�E���E�����v���p
	std::vector<byte>	m_front;		//	�㉺�E���E�����v���F���T���ʒu
	std::vector<byte>	m_list1;
	std::vector<byte>	m_list2;
	std::vector<short>	m_parent_ul;			//	�㍶�ӕ����̐e�Z���C���f�b�N�X�z��
	std::vector<short>	m_parent_dr;			//	���E�ӕ����̐e�Z���C���f�b�N�X�z��
	std::vector<uint64>	m_zobrist_black;		//	���pXOR���]�l�e�[�u��
	std::vector<uint64>	m_zobrist_white;		//	���pXOR���]�l�e�[�u��
	std::unordered_map<uint64, TTEntry>	m_tt;	//	�u���\�iTransposition Table�j
	
	mutable std::vector<short>	m_rave;			//	���v�I�Ȋe�ʒu�ɑł��l�i�������L�j
private:
	// --- ���ԊǗ��p�̃����o�ϐ� ---
	std::chrono::high_resolution_clock::time_point m_startTime;
	int m_timeLimit = 0;
	bool m_timeOver = false;
	long long m_nodesSearched = 0; // �p�t�H�[�}���X�v���p�̒T���m�[�h��
};

bool dfs_black_win(Board&, byte next);		//	�o���őP�ō��������H

