#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <assert.h>
#include "Board.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 
//static std::mt19937 rgen(0); 
std::mt19937_64 rgen64(rd());		// 64ビット版

#define		is_empty()	empty()

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // 番兵つき１次元配列メモリ確保
	m_parent_ul.resize(m_ary_size);
	//m_parent_dr.resize(m_ary_size);
    init();  // 盤面初期化

}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), WALL);	//	for 上下壁
	for(int y = 0; y < m_bd_width; ++y) {
		m_cell[xyToIndex(-1, y)] = WALL;		//	左右壁
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIndex(x, y)] = EMPTY;
	}
	for(int i = 0; i != m_parent_ul.size(); ++i)
		m_parent_ul[i] = i;
	m_parent_ul[TL_INDEX] = TL_INDEX;
	m_parent_ul[BT_INDEX] = BT_INDEX;
	m_parent_ul[RT_INDEX] = RT_INDEX;
	m_last_put_ix = 0;
}
Board& Board::operator=(const Board& s) {
	m_cell = s.m_cell;

	return *this;
}
void Board::print() const {
	cout << "   ";
	for(int x = 0; x < m_bd_width; ++x)
		printf("%c ", 'a'+x);
	cout << endl;
	for(int y = 0; y < m_bd_width; ++y) {
		cout << string(y, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			int ix = xyToIndex(x, y);
			switch(m_cell[ix]) {
			case EMPTY: cout << "・"; break;
			case BLACK: cout << (ix==m_last_put_ix?"★":"●"); break;
			case WHITE: cout << (ix==m_last_put_ix?"☆":"◯"); break;
			default:
				cout << "？";
			}
		}
		cout << endl;
	}
	cout << endl;
}
void Board::print_dist() const {
	for(int y = 0; y < m_bd_width; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_dist[xyToIndex(x, y)];
			if( d == UNCONNECT) cout << "  -1";
			else printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
}
bool Board::is_vert_connected() const {
	m_connected.resize(m_ary_size);
	fill(m_connected.begin(), m_connected.end(), UNSEARCHED);
	for(int x = 0; x < m_bd_width; ++x) {
		if( is_vert_connected_DFS(xyToIndex(x, 0)) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	return false;
}
bool Board::is_vert_connected_DFS(int ix) const {
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	黒でない or 探索済み
		return false;
	if( ix >= xyToIndex(0, m_bd_width-1) )		//	下辺に到達した場合
		return true;
	m_connected[ix] = SEARCHED;
	return	is_vert_connected_DFS(ix + m_ary_width) ||
			is_vert_connected_DFS(ix + m_ary_width - 1) ||
			is_vert_connected_DFS(ix + 1) ||
			is_vert_connected_DFS(ix - 1) ||
			is_vert_connected_DFS(ix - m_ary_width + 1) ||
			is_vert_connected_DFS(ix - m_ary_width);
}
bool Board::is_horz_connected() const {
	m_connected.resize(m_ary_size);
	fill(m_connected.begin(), m_connected.end(), UNSEARCHED);
	for(int y = 0; y < m_bd_width; ++y) {
		if( is_horz_connected_DFS(xyToIndex(0, y)) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	return false;
}
bool Board::is_horz_connected_DFS(int ix) const {
	if( m_cell[ix] != WHITE || m_connected[ix] != UNSEARCHED )	//	黒でない or 探索済み
		return false;
	if( ixToX(ix) == m_bd_width - 1 )		//	右辺に到達した場合
		return true;
	m_connected[ix] = SEARCHED;
	return	is_horz_connected_DFS(ix + m_ary_width) ||
			is_horz_connected_DFS(ix + m_ary_width - 1) ||
			is_horz_connected_DFS(ix + 1) ||
			is_horz_connected_DFS(ix - 1) ||
			is_horz_connected_DFS(ix - m_ary_width + 1) ||
			is_horz_connected_DFS(ix - m_ary_width);
}
//int Board::calc_vert_dist(bool bridge) const {
//	return calc_dist(true, bridge);
//}
//int Board::calc_horz_dist(bool bridge) const {
//	return calc_dist(false, bridge);
//}
//	
int Board::calc_dist(bool vertical, bool bridge, bool rev) const
{
	const Color own_color   = vertical ? BLACK : WHITE;
	const Color opp_color = vertical ? WHITE : BLACK;
	//vector<int> dist(m_ary_size, UNCONNECT);	// 各セルまでの最短距離を格納する配列。UNCONNECTで初期化。
	m_dist.resize(m_ary_size);
	fill(m_dist.begin(), m_dist.end(), UNCONNECT);
	deque<int> q;	// 0-1 BFS のための両端キュー
	// 1. スタート地点（上辺）をキューに追加
	for (int i = 0; i < m_bd_width; ++i) {
		int ix = vertical ? xyToIndex(i, 0) : xyToIndex(0, i);
		if (m_cell[ix] == BLACK) {
			m_dist[ix] = 0;
			q.push_front(ix); // コスト0なので先頭に追加
		} else if(m_cell[ix] == EMPTY) {
			m_dist[ix] = 1;
			q.push_back(ix);  // コスト1なので末尾に追加
		}
	}
	if( bridge ) {
		bool is_emp0 = m_cell[xyToIndex(0, 0)] == EMPTY;	//	空欄か？
		for (int i = 1; i < m_bd_width; ++i) {
			int ix = vertical ? xyToIndex(i, 0) : xyToIndex(0, i);
			bool is_emp = m_cell[ix] == EMPTY;	//	空欄か？
			if( is_emp0 && is_emp ) {
				int ix2 = ix + (vertical ? m_ary_width - 1 : -m_ary_width + 1);
				if (m_cell[ix2] == own_color) {
					m_dist[ix2] = 0;
					q.push_front(ix2); // コスト0なので先頭に追加
				} else if (m_cell[ix2] == EMPTY) {
					m_dist[ix2] = 1;
					q.push_back(ix2);  // コスト1なので末尾に追加
				}
			}
			is_emp0 = is_emp;
		}
	}
    if (q.empty()) {    // もし上辺に有効なマスが一つもなければ連結不可能
        return UNCONNECT;
    }
	const int offsets[] = {	// 隣接セルのインデックス差分
		-1, +1, -m_ary_width, +m_ary_width, -m_ary_width + 1, +m_ary_width - 1
	};
	int min_dist = UNCONNECT;
	while (!q.empty()) {	// 2. 0-1 BFS ループ
		int cur_ix = q.front();
		q.pop_front();
		int current_dist = m_dist[cur_ix];
	
		// 既に下辺へのより短いパスが見つかっている場合、枝刈り
		if (min_dist != UNCONNECT && current_dist >= min_dist) {
			continue;
		}
		// ゴール（下辺 or 右辺）に到達したかチェック
#if 0
		if (vertical && ixToY(cur_ix) == m_bd_width - 1 || !vertical && ixToX(cur_ix) == m_bd_width - 1) {
			//if (min_dist == UNCONNECT || current_dist < min_dist) {
			//	min_dist = current_dist;
			//}
            return current_dist;
		}
#endif
		if( vertical ) {
			int y = ixToY(cur_ix);
			if( y == m_bd_width - 1 ||
				bridge && y == m_bd_width - 2 &&
				m_cell[cur_ix+m_ary_width-1] == EMPTY && m_cell[cur_ix+m_ary_width] == EMPTY )
			{
	            return current_dist;
			}
		} else {
			int x = ixToX(cur_ix);
			if( x == m_bd_width - 1 ||
				bridge && x == m_bd_width - 2 &&
				m_cell[cur_ix-m_ary_width+1] == EMPTY && m_cell[cur_ix+1] == EMPTY )
			{
	            return current_dist;
			}
		}
		for (int offset : offsets) {	// 隣接セルを探索
			int next_ix = cur_ix + offset;
			// 壁・盤外・白石は無視
			if (m_cell[next_ix] == WALL || m_cell[next_ix] == opp_color) {
				continue;
			}
			// 移動コストを計算 (空マスなら1, 黒マスなら0)
			int cost = (m_cell[next_ix] == EMPTY) ? 1 : 0;
			int new_dist = current_dist + cost;
			// より短い経路が見つかった場合のみ更新
			if (m_dist[next_ix] == UNCONNECT || new_dist < m_dist[next_ix]) {
				m_dist[next_ix] = new_dist;
				if (cost == 0) {
					q.push_front(next_ix); // コスト0は先頭へ
				} else {
					q.push_back(next_ix);  // コスト1は末尾へ
				}
			}
		}
		if( bridge ) {
			struct SBridge {
				int		m_emp1;		//	空欄位置へのオフセット
				int		m_emp2;		//	空欄位置へのオフセット
				int		m_next;		//	ブリッジ先へのオフセット
			};
/*
                  -2W+1
        -W-1 -W   -W+1 -W+2
        -1   0    +1
   +W-2 +W-1 +W   +W+1
        2W-1
*/
			const int W = m_ary_width;
			const SBridge offsets[] = {
				{-W, -W+1, -2*W+1},
				{-W+1, 1, -W+2},
				{W, 1, W+1},
				{W, W-1, 2*W-1},
				{-1, W-1, W-2},
				{-1, -W, -W-1},
			};
			for(const auto& ofst: offsets) {
				int emp1_ix = cur_ix + ofst.m_emp1;
				int emp2_ix = cur_ix + ofst.m_emp2;
				int next_ix = cur_ix + ofst.m_next;
				if( m_cell[emp1_ix] != EMPTY || m_cell[emp2_ix] != EMPTY || 
					m_cell[next_ix] == WALL || m_cell[next_ix] == opp_color )
				{
					continue;
				}
				int cost = (m_cell[next_ix] == EMPTY) ? 1 : 0;
				int new_dist = current_dist + cost;
				// より短い経路が見つかった場合のみ更新
				if (m_dist[next_ix] == UNCONNECT || new_dist < m_dist[next_ix]) {
					m_dist[next_ix] = new_dist;
					if (cost == 0) {
						q.push_front(next_ix); // コスト0は先頭へ
					} else {
						q.push_back(next_ix);  // コスト1は末尾へ
					}
				}
			}
		}
	}
	//print_dist();
	return min_dist;
}
void Board::get_empty_indexes(vector<int>& lst) const {
	lst.clear();
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
}
void Board::random_playout(Color next) {
	vector<int> lst;
	get_empty_indexes(lst);
	shuffle(lst.begin(), lst.end(), rgen);
	for(int ix: lst) {
		set_color(ix, next);
		next = (BLACK+WHITE) - next;
	}
	m_last_put_ix = lst.back();
};
bool Board::playout_to_full(Color next) {
	random_playout(next);
	if( next == BLACK )
		return is_vert_connected();
	else
		return !is_vert_connected();
}
bool Board::playout_to_win(Color next) {
	auto n0 = next;
	vector<int> lst;
	get_empty_indexes(lst);
	shuffle(lst.begin(), lst.end(), rgen);
	for(int ix: lst) {
		set_color(ix, next);
#if 0
		if( union_find(ix, next) ) {
			m_last_put_ix = ix;
			break;
		}
#else
		if( next == BLACK && is_vert_connected() ||
			next == WHITE && is_horz_connected() )
		{
			m_last_put_ix = ix;
			break;
		}
#endif
		next = (BLACK+WHITE) - next;
	}
	return next == n0;
};
bool Board::union_find(int ix, Color col) {	//	return: 着手により上下 or 左右辺が連結されたか？
#if 1
	if( col == BLACK ) {
		int x = ixToY(ix);
		if( x == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	上辺に接続
		else if( x == m_bd_width - 1 )
			m_parent_ul[ix] = BT_INDEX;		//	下辺に接続
	} else {	//	col == WHITE
		int y = ixToX(ix);
		if( y == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	左辺に接続
		else if( y == m_bd_width - 1 )
			m_parent_ul[ix] = RT_INDEX;		//	右辺に接続
	}
	check_connected_uf(ix, ix-m_ary_width, col);
	check_connected_uf(ix, ix- m_ary_width +1, col);
	check_connected_uf(ix, ix-1, col);
	check_connected_uf(ix, ix+1, col);
	check_connected_uf(ix, ix+ m_ary_width -1, col);
	check_connected_uf(ix, ix+ m_ary_width, col);
	return m_parent_ul[BT_INDEX] == TL_INDEX || m_parent_ul[RT_INDEX] == TL_INDEX;
#if 0
	//	下辺（右辺）の石が上辺（左辺）に連結しているか？
	if( col == BLACK ) {
		for(int x = 0; x < m_bd_width; ++x) {
			int ix = xyToIndex(x, m_bd_width-1);
			if( m_cell[ix] == BLACK && find_root_ul(ix) == TL_INDEX )
				return true;
		}
	} else {
		for(int y = 0; y < m_bd_width; ++y) {
			int ix = xyToIndex(m_bd_width-1, y);
			if( m_cell[ix] == WHITE && find_root_ul(ix) == TL_INDEX )
				return true;
		}
	}
	return false;
#endif
#else
	if( col == BLACK ) {
		auto y = ixToY(ix);
		if( y == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	上辺に接続
		else if( y == m_bd_width- 1 )
			m_parent_dr[ix] = DR_INDEX;		//	下辺に接続
	} else {	//	col == WHITE
		auto x = ixToX(ix);
		if( x == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	左辺に接続
		else if( x == m_bd_width - 1 )
			m_parent_dr[ix] = DR_INDEX;		//	右辺に接続
	}
	check_connected_uf(ix, ix-m_ary_width, col);
	check_connected_uf(ix, ix- m_ary_width +1, col);
	check_connected_uf(ix, ix-1, col);
	check_connected_uf(ix, ix+1, col);
	check_connected_uf(ix, ix+ m_ary_width -1, col);
	check_connected_uf(ix, ix+ m_ary_width, col);
	if( m_parent_ul[ix] < 0 ) {	//	6近傍に非接続
		m_parent_ul[ix] = ix;
	}
	if( m_parent_dr[ix] < 0 ) {	//	6近傍に非接続
		m_parent_dr[ix] = ix;
	}
	return find_root_ul(ix) == TL_INDEX && find_root_dr(ix) == DR_INDEX;
#endif
}
void Board::check_connected_uf(int ix, int ix2, Color col) {
#if 1
	if( m_cell[ix2] != col ) return;	//	非接続
	if( m_parent_ul[ix] == ix ) {				//	ix が接続処理済みではない
		m_parent_ul[ix] = find_root_ul(ix2);
	} else {		//	ix が接続処理済み → マージ処理
		// それぞれのグループの根（代表ID）を取得
		int root1 = find_root_ul(ix);
		int root2 = find_root_ul(ix2);
		if (root1 != root2) {
			// 根が異なる場合のみマージ、小さい方のIDを根にする
			if (root1 < root2) {
				m_parent_ul[root2] = root1;
			} else {
				m_parent_ul[root1] = root2;
			}
		}
	}
#else
	if( m_cell[ix2] != col ) return;	//	非接続
	if( m_parent_ul[ix] == UNCONNECT ) {				//	ix が接続処理済みではない
		m_parent_ul[ix] = find_root_ul(ix2);
	} else {		//	ix が接続処理済み → マージ処理
		// それぞれのグループの根（代表ID）を取得
		int root1 = find_root_ul(ix);
		int root2 = find_root_ul(ix2);
		if (root1 != root2) {
			// 根が異なる場合のみマージ、小さい方のIDを根にする
			if (root1 < root2) {
				m_parent_ul[root2] = root1;
			} else {
				m_parent_ul[root1] = root2;
			}
		}
	}
	if( m_parent_dr[ix] == UNCONNECT ) {				//	ix が接続処理済みではない
		m_parent_dr[ix] = find_root_dr(ix2);
	} else {		//	ix が接続処理済み → マージ処理
		// それぞれのグループの根（代表ID）を取得
		int root1 = find_root_dr(ix);
		int root2 = find_root_dr(ix2);
		if (root1 != root2) {
			// 根が異なる場合のみマージ、大きい方のIDを根にする
			if (root1 > root2) {
				m_parent_dr[root2] = root1;
			} else {
				m_parent_dr[root1] = root2;
			}
		}
	}
#endif
}
int Board::find_root_ul(int ix) {
	if( m_parent_ul[ix] == ix ) return ix;
	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
	return m_parent_ul[ix] = find_root_ul(m_parent_ul[ix]);
}
//int Board::find_root_dr(int ix) {
//	if(ix < 0 || m_parent_dr[ix] == ix ) return ix;
//	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
//	return m_parent_dr[ix] = find_root_dr(m_parent_dr[ix]);
//}

int Board::sel_move_random() const {
	vector<int> lst;
	get_empty_indexes(lst);
	if( lst.is_empty() ) return -1;
	return lst[rgen() % lst.size()];
}
int Board::sel_move_PMC(Color next, int limit) const {	//	limit: 思考時間 単位：ミリ秒
	vector<int> lst;
	get_empty_indexes(lst);
	if( lst.is_empty() ) return -1;
	Board b2(this->m_bd_width);
	// --- 時間計測の準備 ---
	m_startTime = std::chrono::high_resolution_clock::now();
	m_timeLimit = limit;
	m_timeOver = false;
	m_nodesSearched = 0;
	//
	int np = 0;
	vector<int> nwon(lst.size(), 0);
	for(;;) {
		for(int i = 0; i != lst.size(); ++i) {
			b2 = *this;
			b2.set_color(lst[i], next);
			bool b = b2.playout_to_full((BLACK+WHITE)-next);
			if( !b )
				nwon[i] += 1;
		}
		++np;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
        if (duration >= m_timeLimit)
        	break;
	}
	int best_ix = -1;
	int max_nw = -1;
	for(int i = 0; i != lst.size(); ++i) {
		if( nwon[i] > max_nw ) {
			max_nw = nwon[i];
			best_ix = lst[i];
		}
	}
	cout << "n_won / n_playout = " << max_nw << "/" << np << " = " << (double)max_nw/np <<  endl << endl;
	return best_ix;
}
void Board::do_DFS(Color next, int depth) {		//	depth == 0 になるまで深さ優先探索
	m_nodesSearched = 0;
	DFS_recursive(next, depth);
}
void Board::DFS_recursive(Color next, int depth) {		//	depth == 0 になるまで深さ優先探索
	if( depth == 0 ) {
		m_nodesSearched += 1;
		return;
	}
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			DFS_recursive((BLACK+WHITE)-next, depth-1);
			m_cell[ix] = EMPTY;
		}
	}
}
int Board::do_itrdeep(Color next, int limit) {		//	
	build_zobrist_table();
	m_hash_val = 0;			//	現局面のハッシュ値を０に
	m_tt.clear();			//	置換表クリア
	// --- 時間計測の準備 ---
	m_startTime = std::chrono::high_resolution_clock::now();
	m_timeLimit = limit;
	m_timeOver = false;
	m_nodesSearched = 0;
	for(int depth = 1; ; ++depth) {
		itrdeep_recursive(next, depth);
		if( m_timeOver ) {
			return depth;
		}
	}
}
void Board::itrdeep_recursive(Color next, int depth) {		//	depth == 0 になるまで深さ優先探索
	if ((++m_nodesSearched & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
        if (duration >= m_timeLimit) {
            m_timeOver = true;
            return;
        }
    }
	if( depth == 0 ) {
		return;
	}
	TTEntry& entry = m_tt[m_hash_val];		//	現局面が未登録の場合は、要素が自動的に追加される
	if( entry.m_depth >= depth )			//	現局面は探索済み
		return;
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
			itrdeep_recursive((BLACK+WHITE)-next, depth-1);
			m_cell[ix] = EMPTY;
			m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
			if( m_timeOver ) break;
		}
	}
	entry.m_depth = depth;
}
void Board::build_zobrist_table() {
	if( !m_zobrist_black.is_empty() ) return;	//	初期化済み
	m_zobrist_black.resize(m_ary_size);
	for(auto& r: m_zobrist_black) r = rgen64();
	m_zobrist_white.resize(m_ary_size);
	for(auto& r: m_zobrist_white) r = rgen64();
}
