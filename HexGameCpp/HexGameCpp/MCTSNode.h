#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include "Board.h"

using byte = unsigned char;
using ushort = unsigned short;

const double EXPLORATION_CONSTANT = 1.414;

class MCTSNode {
public:
    MCTSNode* m_parent;
    std::vector<MCTSNode*> m_children;
    std::vector<int> m_untried_moves; // ���̃m�[�h����܂������Ă��Ȃ��i�W�J���Ă��Ȃ��j���@��

	int		m_move;			// �e�m�[�h���炱�̃m�[�h�Ɏ��钅��
	byte	m_player_to_move;	// ���̃m�[�h�̋ǖʂ��璅�肷��v���C���[�F
	int		m_wins;			   // ���̃m�[�h���o�R�����V�~�����[�V�����ł̏������i���̃m�[�h���_�j
	int		m_visits;			// ���̃m�[�h�̖K���
	bool	m_is_terminal;		// ���̃m�[�h���Q�[���I���ǖʂ�

public:
    MCTSNode(const Board& board, byte player_to_move, MCTSNode* parent = nullptr, int move = -1, bool is_win_move = false);
    ~MCTSNode();

    void print(int lvl = 0) const;
    void print_best() const;
    MCTSNode* select_child_UCT() const;
    MCTSNode* expand(Board& board);
    void update(byte win_col);
};
