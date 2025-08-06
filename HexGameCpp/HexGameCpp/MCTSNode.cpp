#include <iostream>
#include <string>
#include "MCTSNode.h"

using namespace std;

const Board *g_bd;

// コンストラクタ
MCTSNode::MCTSNode(const Board& board, byte player_to_move, MCTSNode* parent, int move, bool is_win_move)
    : m_parent(parent), m_move(move), m_player_to_move(player_to_move), m_wins(0), m_visits(0), m_is_terminal(is_win_move) {
    g_bd = &board;
    if (!m_is_terminal) {
        board.get_empty_list(m_untried_moves);
    }
}

// デストラクタ（子ノードを再帰的に削除）
MCTSNode::~MCTSNode() {
    for (MCTSNode* child : m_children) {
        delete child;
    }
}

void MCTSNode::print(int lvl) const {
	string spc(lvl*2, ' ');
	cout << spc << "move: " << m_move;
	cout << ", " << m_wins << "/" << m_visits;
	cout << ", next: " << (int)m_player_to_move << endl;
	for(int i = 0; i < m_children.size(); ++i) {
		m_children[i]->print(lvl+1);
	}
}
void MCTSNode::print_best() const {
    if (m_children.is_empty()) return;
	//int best_ix = -1;
	const MCTSNode *best_node = nullptr;
	//int max_visits = -1;
	double max_rate = -1;
	for(const auto* ptr: m_children) {
		//if( ptr->m_visits > max_visits )
		if( (double)ptr->m_wins / ptr->m_visits > max_rate )
		{
			max_rate = (double)ptr->m_wins / ptr->m_visits;
			//max_visits = ptr->m_visits;
			best_node = ptr;
		}
	}
	printf("%c%d", 'a'+g_bd->indexToX(best_node->m_move), g_bd->indexToY(best_node->m_move)+1);
	printf("(%d/%d=%.3f), ", best_node->m_wins, best_node->m_visits, (double)best_node->m_wins/best_node->m_visits);
	//cout << "(" << g_bd->indexToX(best_node->m_move) << ", " << g_bd->indexToY(best_node->m_move) << ") ";
	//cout << "(" << best_node->m_wins << "/" << best_node->m_visits << "), ";
	if( best_node->m_is_terminal )
		cout << endl;
	else
		best_node->print_best();
}

// UCT (Upper Confidence Bound for Trees) を使って最も有望な子ノードを選択
MCTSNode* MCTSNode::select_child_UCT() const {
    MCTSNode* best_child = nullptr;
    double best_score = -1.0;

    for (MCTSNode* child : m_children) {
        // UCT値を計算
        // (勝利数 / 訪問回数) + C * sqrt(log(親の訪問回数) / 自分の訪問回数)
        // 常に黒番視点の勝利数で計算し、白番のときは白の勝率が最大になる手を選ぶ
        double win_rate = (double)child->m_wins / child->m_visits;	//	child ノードの勝率
        //if (m_player_to_move == BLACK) {
        //    win_rate = (double)child->m_wins / child->m_visits;
        //} else { // WHITE
        //    win_rate = 1.0 - (child->m_wins / child->m_visits); // 白の勝率 = 1 - 黒の勝率
        //}
		double t = EXPLORATION_CONSTANT * std::sqrt(std::log((double)m_visits) / child->m_visits);
        double uct_score = win_rate + t;

        if (uct_score > best_score) {
            best_score = uct_score;
            best_child = child;
        }
    }
    return best_child;
}

// 未試行の手を一つ展開し、新しい子ノードを作成して返す
MCTSNode* MCTSNode::expand(Board& board) {
    // 未試行の手を一つ取り出す
    int move = m_untried_moves.back();
    m_untried_moves.pop_back();

    // 盤面をコピーせず、引数で渡された盤面を進める
    bool is_win = board.put_and_check_uf(move, m_player_to_move);

    // 次の手番のプレイヤー
    byte next_player = (m_player_to_move == BLACK) ? WHITE : BLACK;

    // 新しい子ノードを作成
    MCTSNode* new_child = new MCTSNode(board, next_player, this, move, is_win);
    m_children.push_back(new_child);
    return new_child;
}

// シミュレーション結果をバックプロパゲーションで更新
// resultは黒の勝利なら1.0, 白の勝利なら0.0
void MCTSNode::update(byte win_col) {
    auto *node = this;
    while( node != nullptr ) {
	    node->m_visits += 1;
	    if( win_col != node->m_player_to_move )
		    node->m_wins += 1;		//	手番側が勝った回数（m_player_to_move は次の手番）
	    node = node->m_parent;
    }
}
