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
	bool	put_color(int x, int y, byte col);		//	return: ����ɂ��㉺ or ���E�ӂ��A�����ꂽ
	bool	put_and_check(int x, int y, byte col);
	void	check_connected(int ix, int ix2, byte col);
	bool	put_and_check_uf(int ix, byte col);
	void	check_connected_uf(int ix, int ix2, byte col);
	int		find_root(int gid);
	int		find_root_ul(int ix);
	int		find_root_dr(int ix);

	bool	is_vert_connected();			//	�㉺�ӂ��A������Ă��邩�H �󗓂�������ԂŃR�[�������
	int		calc_vert_dist(bool ex=true);	//	�i���j�㉺�ӊԋ����v�Z�i�U�A���{�ԐژA��(ex)�j
	int		calc_horz_dist(bool ex=true);	//	�i���j���E�ӊԋ����v�Z�i�U�A���{�ԐژA��(ex)�j
	//int		calc_vert_dist_ex();	//	�i���j�㉺�ӊԋ����v�Z�i�U�A���{�ԐژA���j
	//int		calc_horz_dist_ex();	//	�i���j���E�ӊԋ����v�Z�i�U�A���{�ԐژA���j
	int		find_winning_move_black();
	int		find_winning_move_white();
	void	get_empty_list(std::vector<int>&) const;

	int		eval();				//	�����猩���]���l���v�Z
	int		alpha_beta_black(int alpha, int beta, int depth);
	int		alpha_beta_white(int alpha, int beta, int depth);
	int		black_turn(int depth);
	int		white_turn(int depth);

	void	playout_to_end(byte next);	//	���S�����_���v���C�A�E�g
	bool	playout(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	byte	playout_smart(byte next);	//	���S�����_���v���C�A�E�g�Areturn: BLACK | WHITE�A���s����������v�Z
	double	playout_smart(int N, byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	bool	playout_old(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	bool	playout_rave(byte next) const;	//	���S�����_���v���C�A�E�g�Areturn: ������
	double	estimate_win_rate_PMC(byte next, int N) const;	//	���S�����_���v���C�A�E�g�Ŏ���ԏ��������߂�
	bool	did_black_win(int ix);		//	�� ix �ɑł��āA�㉺�ӂ��A�����ꂽ���H
	int		sel_move_random();			//	���S�����_���ɒ����I���Areturn: ����ӏ�
	int		sel_move_PMC(byte next);		//	���������e�J�����@�Œ����I���Areturn: ����ӏ�
	int		sel_move_MCTS(byte next);		//	�����e�J�����ؒT���Œ����I���Areturn: ����ӏ�
	int		sel_move_win(byte next);		//	1��ŏ������m�肷��肪����΂����Ԃ��A������� -1 ��Ԃ�
	int		sel_move_block(byte next);		//	����̏�������u���b�N����肪����΂����Ԃ��A������� -1 ��Ԃ�
private:
	bool	is_vert_connected_sub(int ix);
	void	calc_dist_sub(int ix, int dix, ushort dist, byte col);
	void	calc_dist_sub2(int ix, int ix2, int ix3, int dix, ushort dist, byte col);

public:
	int		m_bd_width;					//	�Ֆʕ�
	int		m_bd_height;				//	�Ֆʍ��i�ʏ�� m_bd_width �Ɠ��l�j
	int		m_ary_width;				//	���͂̕ǁi�Ԑl�j���܂ޔՖʕ�
	int		m_ary_height;				//	���͂̕ǁi�Ԑl�j���܂ޔՖʍ�
	int		m_ary_size;					//	�P�����Ֆʔz��T�C�Y
	int		DR_INDEX;
	short	m_next_gid;
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
	mutable std::vector<short>	m_rave;			//	���v�I�Ȋe�ʒu�ɑł��l�i�������L�j
};

bool dfs_black_win(Board&, byte next);		//	�o���őP�ō��������H

