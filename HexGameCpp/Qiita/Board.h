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
	bool	is_vert_connected() const;		//	�㉺�ӂ��A�����Ă��邩�H
	bool	is_horz_connected() const;		//	���E�ӂ��A�����Ă��邩�H
	void	random_playout(Color next);
	bool	playout_to_full(Color next);		//	�󗓂������Ȃ�܂Ńv���C�A�E�g���Anext �����������ǂ�����Ԃ�
	bool	playout_to_win(Color next);		//	���s�����܂�܂Ńv���C�A�E�g���Anext �����������ǂ�����Ԃ�

	int		sel_move_random() const;
	int		sel_move_PMC(Color next, int limit=1000) const;	//	limit: �v�l���� �P�ʁF�~���b
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
	//const int	UL_INDEX = 0;					//	�㍶���z���[�g for Union-Find
	//int			DR_INDEX;						//	���E�Ӊ��z���[�g for Union-Find
	int		m_last_put_ix = 0;
	std::vector<Color>	m_cell;					//	�e�Z����ԁi��E���E���j
	std::vector<short>	m_parent_ul;			//	�㍶�ӕ����̐e�Z���C���f�b�N�X�z��
	std::vector<short>	m_parent_dr;			//	���E�ӕ����̐e�Z���C���f�b�N�X�z��
	mutable std::vector<byte>	m_connected;
};

