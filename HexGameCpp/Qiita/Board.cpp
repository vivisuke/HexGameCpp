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

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // 番兵つき１次元配列メモリ確保
	m_parent_ul.resize(m_ary_size);
	m_parent_dr.resize(m_ary_size);
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
	m_parent_dr[BT_INDEX] = BT_INDEX;
	m_parent_dr[RT_INDEX] = RT_INDEX;
	m_last_put_ix = 0;
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
#if 1
		if( union_find(ix, next) ) {
			m_last_put_ix = ix;
			break;
		}
#else
		if( next == BLACK && is_vert_connected() ||
			next == WHITE && is_horz_connected() )
		{
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
int Board::find_root_dr(int ix) {
	if(ix < 0 || m_parent_dr[ix] == ix ) return ix;
	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
	return m_parent_dr[ix] = find_root_dr(m_parent_dr[ix]);
}
