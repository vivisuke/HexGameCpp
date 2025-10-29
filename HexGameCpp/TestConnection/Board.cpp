#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <queue>
#include <assert.h>
#include "Board.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 
//static std::mt19937 rgen(0); 
std::mt19937_64 rgen64(rd());		// 64ビット版

#define		is_empty()	empty()

struct SBridge {
	int		m_emp1;		//	空欄位置へのオフセット
	int		m_emp2;		//	空欄位置へのオフセット
	int		m_next;		//	ブリッジ先へのオフセット
};

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // 番兵つき１次元配列メモリ確保
	m_parent_ul.resize(m_ary_size);
    init();  // 盤面初期化
	build_fixed_order();
	build_zobrist_table();
}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), WALL);	//	for 上下壁
	for(int y = 0; y < m_bd_width; ++y) {
		m_cell[xyToIX(-1, y)] = WALL;		//	左右壁
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIX(x, y)] = EMPTY;
	}
	for(int i = 0; i != m_parent_ul.size(); ++i)
		m_parent_ul[i] = i;
	m_parent_ul[TL_INDEX] = TL_INDEX;
	m_parent_ul[BT_INDEX] = BT_INDEX;
	m_parent_ul[RT_INDEX] = RT_INDEX;
	m_uf_stack.clear();
	m_uf_stack.push_back(0);		//	undo 情報区切り
	m_last_put_ix = 0;
}
void Board::build_fixed_order() {
	m_fixed_order.clear();
	for(int x = 0; x < m_bd_width; ++x) {
		int y = m_bd_width-1;
		//cout << "(" << x << ", " << y << ")" << endl;
		build_fixed_order_sub(xyToIX(x, y), m_bd_width-x);
		if( x != 0 ) {
			int y = m_bd_width - x - 1;
			//cout << "(0, " << y << ")" << endl;
			build_fixed_order_sub(xyToIX(0, y), m_bd_width-x);
		}
	}
}
void Board::build_zobrist_table() const {
	if( !m_zobrist_black.is_empty() ) return;	//	初期化済み
	m_zobrist_black.resize(m_ary_size);
	for(auto& r: m_zobrist_black) r = rgen64();
	m_zobrist_white.resize(m_ary_size);
	for(auto& r: m_zobrist_white) r = rgen64();
}
void Board::build_fixed_order_sub(int ix, int len) {
	if( len%2 == 0 ) {	//	len が偶数の場合
		int cix = ix - (m_ary_width-1) * (len/2-1);
		for(int i = 0; i < len/2; ++i) {
			m_fixed_order.push_back(cix);
			m_fixed_order.push_back(cix - (m_ary_width-1)*(i*2 + 1));
			//cout << cix << " " << cix - (m_ary_width-1)*(i*2 + 1) << " ";
			cix += m_ary_width-1;
		}
		//cout << endl;
	} else {	//	len が奇数の場合
		int cix = ix - (m_ary_width-1) * (len/2);
		m_fixed_order.push_back(cix);
		//cout << cix << " ";
		for(int i = 0; i < len/2; ++i) {
			cix += m_ary_width-1;
			m_fixed_order.push_back(cix);
			m_fixed_order.push_back(cix - (m_ary_width-1)*(i*2 + 2));
			//cout << cix << " " << cix - (m_ary_width-1)*(i*2 + 2) << " ";
		}
		//cout << endl;
	}
}
Board& Board::operator=(const Board& s) {
	m_cell = s.m_cell;
	return *this;
}
void Board::print() const {
	auto c = next_color();
	if( c == BLACK ) cout << "next: BLACK" << endl;
	else cout << "next: WHITE" << endl;
	cout << "   ";
	for(int x = 0; x < m_bd_width; ++x)
		printf("%c ", 'a'+x);
	cout << endl;
	for(int y = 0; y < m_bd_width; ++y) {
		cout << string(y, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			int ix = xyToIX(x, y);
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
void Board::print_parent_ul() const {
	for(int ix = 0; ix < m_ary_size; ++ix) {
		printf("%3d", m_parent_ul[ix]);
		if( ixToX(ix) == m_ary_width - 1 )
			cout << endl;
	}
}
Color Board::next_color() const {
	int nb = 0, nw = 0;
	for(int ix = xyToIX(0, 0); ix <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == BLACK ) ++nb;
		else if( m_cell[ix] == WHITE ) ++nw;
	}
	if( nb <= nw ) return BLACK;
	return WHITE;
}
int Board::n_empty() const {
	int n = 0;
	for(int ix = xyToIX(0, 0); ix <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			++n;
	}
	return n;
}
void Board::get_empty_indexes(vector<int>& lst) const {
	lst.clear();
	for(int ix = xyToIX(0, 0); ix <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
}
bool Board::is_vert_connected() const {
	m_connected.resize(m_ary_size);
	fill(m_connected.begin(), m_connected.end(), UNSEARCHED);
	for(int x = 0; x < m_bd_width; ++x) {
		if( is_vert_connected_DFS(xyToIX(x, 0)) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	return false;
}
bool Board::is_vert_connected_DFS(int ix) const {
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	黒でない or 探索済み
		return false;
	if( ix >= xyToIX(0, m_bd_width-1) )		//	下辺に到達した場合
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
		if( is_horz_connected_DFS(xyToIX(0, y)) )		//	深さ優先探索
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
bool Board::is_vert_connected_v() const {
	m_connected.resize(m_ary_size);
	fill(m_connected.begin(), m_connected.end(), UNSEARCHED);
	for(int x = 0; x < m_bd_width-1; ++x) {
		int ix = xyToIX(x, 1);
		if( is_vert_connected_v_DFS(ix, ix-m_ary_width, ix-m_ary_width+1) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	for(int x = 0; x < m_bd_width; ++x) {
		if( is_vert_connected_v_DFS(xyToIX(x, 0)) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	return false;
}
#define	UU	(-2*W+1)
#define	ULL	(-W-1)
#define	UL	(-W)
#define	UR	(-W+1)
#define	URR	(-W+2)
#define	LT	(-1)
#define	RT	(+1)
#define	DLL	(+W-2)
#define	DL	(+W-1)
#define	DR	(+W)
#define	DRR	(+W+1)
#define	DD	(+2*W-1)
/*
          UU
	ULL UL  UR  URR
	  LT  ０  RT
	DLL DL  DR  DRR
	      DD

                  -2W+1
        -W-1 -W   -W+1 -W+2
        -1   0    +1
   +W-2 +W-1 +W   +W+1
        +2W-1
*/

bool Board::is_vert_connected_v_DFS(int ix, int mx1, int mx2) const {
	if (mx1 >= 0 && (m_cell[mx1] != EMPTY || m_cell[mx2] != EMPTY))	//	見合い位置が空欄で無い
		return false;
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	黒でない or 探索済み
		return false;
	if( ixToY(ix) == m_bd_width-2 && m_cell[ix+m_ary_width-1] == EMPTY && m_cell[ix+m_ary_width] == EMPTY )
		return true;						//	下辺に見合い連結
	if( ix >= xyToIX(0, m_bd_width-1) )		//	下辺に到達した場合
		return true;
	m_connected[ix] = SEARCHED;
	const auto W = m_ary_width;
#if 0
	return	is_horz_connected_v_DFS(ix + 2*W - 1, ix + W - 1, ix + W) ||
			is_horz_connected_v_DFS(ix + W - 2, ix + W - 1, ix - 1) ||
			is_horz_connected_v_DFS(ix + W + 1, ix + 1, ix + W) ||
			is_horz_connected_v_DFS(ix - W + 2, ix - W + 1, ix + 1) ||
			is_horz_connected_v_DFS(ix - W - 1, ix - W, ix - 1) ||
			is_horz_connected_v_DFS(ix - 2*W + 1, ix - W, ix - W + 1) ||
			is_vert_connected_v_DFS(ix + m_ary_width) ||
			is_vert_connected_v_DFS(ix + m_ary_width - 1) ||
			is_vert_connected_v_DFS(ix + 1) ||
			is_vert_connected_v_DFS(ix - 1) ||
			is_vert_connected_v_DFS(ix - m_ary_width + 1) ||
			is_vert_connected_v_DFS(ix - m_ary_width);
#else
	if( is_vert_connected_v_DFS(ix + DD, ix + DL, ix + DR) ) return true;
	if( is_vert_connected_v_DFS(ix + DLL, ix + DL, ix + LT) ) return true;
	if( is_vert_connected_v_DFS(ix + DRR, ix + DR, ix + RT) ) return true;
	if( is_vert_connected_v_DFS(ix + ULL, ix + UL, ix + LT) ) return true;
	if( is_vert_connected_v_DFS(ix + URR, ix + UR, ix + RT) ) return true;
	if( is_vert_connected_v_DFS(ix + UU, ix + UL, ix + UR) ) return true;
	if( is_vert_connected_v_DFS(ix + m_ary_width) ) return true;
	if( is_vert_connected_v_DFS(ix + m_ary_width - 1) ) return true;
	if( is_vert_connected_v_DFS(ix + 1) ) return true;
	if( is_vert_connected_v_DFS(ix - 1) ) return true;
	if( is_vert_connected_v_DFS(ix - m_ary_width + 1) ) return true;
	if( is_vert_connected_v_DFS(ix - m_ary_width) ) return true;
	return false;
#endif
}
bool Board::is_horz_connected_v() const {
	m_connected.resize(m_ary_size);
	fill(m_connected.begin(), m_connected.end(), UNSEARCHED);
	for(int y = 0; y < m_bd_width-1; ++y) {
		int ix = xyToIX(1, y);
		if( is_horz_connected_v_DFS(ix, ix-1, ix+m_ary_width-1) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	for(int y = 0; y < m_bd_width; ++y) {
		if( is_horz_connected_v_DFS(xyToIX(0, y)) )		//	深さ優先探索
			return true;	//	上下連結経路を発見した場合
	}
	return false;
}
bool Board::is_horz_connected_v_DFS(int ix, int mx1, int mx2) const {
	if (mx1 >= 0 && (m_cell[mx1] != EMPTY || m_cell[mx2] != EMPTY))	//	見合い位置が空欄で無い
		return false;
	if( m_cell[ix] != WHITE || m_connected[ix] != UNSEARCHED )	//	白でない or 探索済み
		return false;
	int x = ixToX(ix);
	if( x == m_bd_width-2 && m_cell[ix+1] == EMPTY && m_cell[ix-m_ary_width+1] == EMPTY )
		return true;						//	右辺に見合い連結
	if( x == m_bd_width-1 )		//	右辺に到達した場合
		return true;
	m_connected[ix] = SEARCHED;
	const auto W = m_ary_width;
#if 0
	return	is_horz_connected_v_DFS(ix + 2*W - 1, ix + W - 1, ix + W) ||
			is_horz_connected_v_DFS(ix + W - 2, ix + W - 1, ix - 1) ||
			is_horz_connected_v_DFS(ix + W + 1, ix + 1, ix + W) ||
			is_horz_connected_v_DFS(ix - W + 2, ix - W + 1, ix + 1) ||
			is_horz_connected_v_DFS(ix - W - 1, ix - W, ix - 1) ||
			is_horz_connected_v_DFS(ix - 2*W + 1, ix - W, ix - W + 1) ||
			is_horz_connected_v_DFS(ix + m_ary_width) ||
			is_horz_connected_v_DFS(ix + m_ary_width - 1) ||
			is_horz_connected_v_DFS(ix + 1) ||
			is_horz_connected_v_DFS(ix - 1) ||
			is_horz_connected_v_DFS(ix - m_ary_width + 1) ||
			is_horz_connected_v_DFS(ix - m_ary_width);
#else
	if( is_horz_connected_v_DFS(ix + 2*W - 1, ix + W - 1, ix + W) ) return true;
	if( is_horz_connected_v_DFS(ix + W - 2, ix + W - 1, ix - 1) ) return true;
	if( is_horz_connected_v_DFS(ix + W + 1, ix + 1, ix + W) ) return true;
	if( is_horz_connected_v_DFS(ix - W + 2, ix - W + 1, ix + 1) ) return true;
	if( is_horz_connected_v_DFS(ix - W - 1, ix - W, ix - 1) ) return true;
	if( is_horz_connected_v_DFS(ix - 2*W + 1, ix - W, ix - W + 1) ) return true;
	if( is_horz_connected_v_DFS(ix + m_ary_width) ) return true;
	if( is_horz_connected_v_DFS(ix + m_ary_width - 1) ) return true;
	if( is_horz_connected_v_DFS(ix + 1) ) return true;
	if( is_horz_connected_v_DFS(ix - 1) ) return true;
	if( is_horz_connected_v_DFS(ix - m_ary_width + 1) ) return true;
	if( is_horz_connected_v_DFS(ix - m_ary_width) ) return true;
	return false;
#endif
}
void Board::undo_union_find() {
	m_uf_stack.pop_back();
	int ix;
	while( (ix = m_uf_stack.back()) != 0 ) {
		m_uf_stack.pop_back();
		auto old = m_uf_stack.back();
		m_uf_stack.pop_back();
		m_parent_ul[ix] = old;
	}
}
bool Board::union_find(int ix, Color col) {	//	return: 着手により上下 or 左右辺が連結されたか？
	if( col == BLACK ) {
		int x = ixToY(ix);
		if( x == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	上辺に接続
		} else if( x == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = BT_INDEX;		//	下辺に接続
		}
	} else {	//	col == WHITE
		int y = ixToX(ix);
		if( y == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	左辺に接続
		} else if( y == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = RT_INDEX;		//	右辺に接続
		}
	}
	check_connected_uf(ix, ix-m_ary_width, col);
	check_connected_uf(ix, ix- m_ary_width +1, col);
	check_connected_uf(ix, ix-1, col);
	check_connected_uf(ix, ix+1, col);
	check_connected_uf(ix, ix+ m_ary_width -1, col);
	check_connected_uf(ix, ix+ m_ary_width, col);
	m_uf_stack.push_back(0);
	return m_parent_ul[BT_INDEX] == TL_INDEX || m_parent_ul[RT_INDEX] == TL_INDEX;
}
void Board::check_connected_uf(int ix, int ix2, Color col) {
	if( m_cell[ix2] != col ) return;	//	非接続
	if( m_parent_ul[ix] == ix ) {				//	ix が接続処理済みではない
		m_uf_stack.push_back(m_parent_ul[ix]);
		m_uf_stack.push_back(ix);
		m_parent_ul[ix] = find_root_ul(ix2);
	} else {		//	ix が接続処理済み → マージ処理
		// それぞれのグループの根（代表ID）を取得
		int root1 = find_root_ul(ix);
		int root2 = find_root_ul(ix2);
		if (root1 != root2) {
			// 根が異なる場合のみマージ、小さい方のIDを根にする
			if (root1 < root2) {
				m_uf_stack.push_back(m_parent_ul[root2]);
				m_uf_stack.push_back(root2);
				m_parent_ul[root2] = root1;
			} else {
				m_uf_stack.push_back(m_parent_ul[root1]);
				m_uf_stack.push_back(root1);
				m_parent_ul[root1] = root2;
			}
		}
	}
}
#if 0
bool Board::union_find_v(int ix, Color col) {	//	return: 着手により上下 or 左右辺が連結されたか？
	if( col == BLACK ) {
		int x = ixToY(ix);
		if( x == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	上辺に接続
		} else if( x == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = BT_INDEX;		//	下辺に接続
		}
	} else {	//	col == WHITE
		int y = ixToX(ix);
		if( y == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	左辺に接続
		} else if( y == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = RT_INDEX;		//	右辺に接続
		}
	}
	check_connected_uf_v(ix, ix-m_ary_width, col);
	check_connected_uf_v(ix, ix- m_ary_width +1, col);
	check_connected_uf_v(ix, ix-1, col);
	check_connected_uf_v(ix, ix+1, col);
	check_connected_uf_v(ix, ix+ m_ary_width -1, col);
	check_connected_uf_v(ix, ix+ m_ary_width, col);
	m_uf_stack.push_back(0);
	return m_parent_ul[BT_INDEX] == TL_INDEX || m_parent_ul[RT_INDEX] == TL_INDEX;
}
#endif
int Board::find_root_ul(int ix) {
	if( m_parent_ul[ix] == ix ) return ix;
	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
	//return m_parent_ul[ix] = find_root_ul(m_parent_ul[ix]);
	auto root = find_root_ul(m_parent_ul[ix]);
	if( root != m_parent_ul[ix] ) {
		m_uf_stack.push_back(m_parent_ul[ix]);
		m_uf_stack.push_back(ix);
		m_parent_ul[ix] = root;
	}
	return root;
}
int Board::calc_dist(bool vertical, bool bridge, bool rev) const
{
	const Color own_color   = vertical ? BLACK : WHITE;
	const Color opp_color = vertical ? WHITE : BLACK;
	//vector<int> dist(m_ary_size, UNCONNECT);	// 各セルまでの最短距離を格納する配列。UNCONNECTで初期化。
	m_dist.resize(m_ary_size);
	fill(m_dist.begin(), m_dist.end(), UNCONNECT);
	deque<int> q;	// 0-1 BFS のための両端キュー
	// 1. スタート地点（上・左辺）をキューに追加
	for (int i = 0; i < m_bd_width; ++i) {
		int ix = vertical ? xyToIX(i, 0) : xyToIX(0, i);
		if (m_cell[ix] == own_color) {
			m_dist[ix] = 0;
			q.push_front(ix); // コスト0なので先頭に追加
		} else if(m_cell[ix] == EMPTY) {
			m_dist[ix] = 1;
			q.push_back(ix);  // コスト1なので末尾に追加
		}
	}
	if( bridge ) {
		bool is_emp0 = m_cell[xyToIX(0, 0)] == EMPTY;	//	空欄か？
		for (int i = 1; i < m_bd_width; ++i) {
			int ix = vertical ? xyToIX(i, 0) : xyToIX(0, i);
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
//	ix に col を打って、勝てるか？
//	固定順序付け、一手ごとに見合い連結チェック
bool Board::is_winning_move(int ix, Color col) {
	m_cell[ix] = col;
	bool b = col == BLACK ? is_vert_connected_v() : is_horz_connected_v();
	if( !b ) {		//	１手で勝ちでない場合
		//b = !is_winning_position(oppo_color(col));
		for(int ix2: m_fixed_order) {
			if( m_cell[ix2] == EMPTY ) {
				b = !is_winning_move(ix2, oppo_color(col));
				if( !b ) break;
			}
		}
	}
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_position(Color next) {
	return false;
}
//	固定順序付け、一手ごとに見合い連結チェック
bool Board::is_winning_move_dist1(int ix, Color col, bool depth1) {
	m_cell[ix] = col;
	bool b = false;
	if( depth1 )
		b = col == BLACK ? is_vert_connected_v() : is_horz_connected_v();
	if( !b ) {		//	相手が勝ちでない場合
		int dist = calc_dist(col==WHITE, true, false);
		if( dist <= 1 )
			b = false;
		else {
			for(int ix2: m_fixed_order) {
				if( m_cell[ix2] == EMPTY ) {
					b = !is_winning_move_dist1(ix2, oppo_color(col), false);
					if( !b ) break;
				}
			}
		}
	}
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_move_TT(int ix, Color col) {
	m_cell[ix] = col;
	m_hash_val ^= (col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix]);
	bool b = !is_winning_position_TT(oppo_color(col));
	m_hash_val ^= col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_position_TT(Color next) {
	TT2Entry& entry = m_tt2[m_hash_val];		//	現局面が未登録の場合は、要素が自動的に追加される
	if( entry.m_flag == FLAG_EXACT )
		return entry.m_winning;
	if( (next == WHITE ? is_vert_connected_v() : is_horz_connected_v()) )
		return false;		//	直前手番の方が勝ち
	bool b = false;
	for(int ix: m_fixed_order) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			m_hash_val ^= (next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix]);
			b = !is_winning_position_TT( oppo_color(next));
			m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
			m_cell[ix] = EMPTY;
			if (b) break;
		}
	}
	entry.m_flag = FLAG_EXACT;
	entry.m_winning = b;
	return b;
}
bool Board::is_winning_move_dist1_TT(int ix, Color col) {
	m_cell[ix] = col;
	m_hash_val ^= (col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix]);
	bool b = !is_winning_position_dist1_TT(oppo_color(col));
	m_hash_val ^= col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_position_dist1_TT(Color next, bool depth1) {
	TT2Entry& entry = m_tt2[m_hash_val];		//	現局面が未登録の場合は、要素が自動的に追加される
	if( entry.m_flag == FLAG_EXACT )
		return entry.m_winning;
	if( depth1 && (next == WHITE ? is_vert_connected_v() : is_horz_connected_v()) )
		return false;		//	直前手番の方が勝ち
	bool b = false;
	int dist = calc_dist(next==BLACK, true, false);
	if( dist <= 1 )
		b = true;
	else {
		for(int ix: m_fixed_order) {
			if( m_cell[ix] == EMPTY ) {
				m_cell[ix] = next;
				m_hash_val ^= (next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix]);
				b = !is_winning_position_dist1_TT(oppo_color(next), false);
				m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
				m_cell[ix] = EMPTY;
				if (b) break;
			}
		}
	}
	entry.m_flag = FLAG_EXACT;
	entry.m_winning = b;
	return b;
}
