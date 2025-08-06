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
    std::vector<int> m_untried_moves; // このノードからまだ試していない（展開していない）合法手

	int		m_move;			// 親ノードからこのノードに至る着手
	byte	m_player_to_move;	// このノードの局面から着手するプレイヤー色
	int		m_wins;			   // このノードを経由したシミュレーションでの勝利数（このノード視点）
	int		m_visits;			// このノードの訪問回数
	bool	m_is_terminal;		// このノードがゲーム終了局面か

public:
    MCTSNode(const Board& board, byte player_to_move, MCTSNode* parent = nullptr, int move = -1, bool is_win_move = false);
    ~MCTSNode();

    void print(int lvl = 0) const;
    void print_best() const;
    MCTSNode* select_child_UCT() const;
    MCTSNode* expand(Board& board);
    void update(byte win_col);
};
