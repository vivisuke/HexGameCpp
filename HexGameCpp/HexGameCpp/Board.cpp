#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <assert.h>
#include "Board.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 
//static std::mt19937 rgen(3); 

#define		is_empty()	empty()

vector<int> g_lst;

Board::Board(int bd_width)
	: m_bd_width(bd_width), m_bd_height(bd_width)
{
	m_ary_width = m_bd_width + 1;
	m_ary_height = m_bd_width + 2;
	m_ary_size = m_ary_width * m_ary_height;
	DR_INDEX = xyToIndex(0, m_bd_height);
	m_cell.resize(m_ary_size);
	m_gid.resize(m_ary_size);
	m_dist.resize(m_ary_size);
	m_parent_ul.resize(m_ary_size);
	m_parent_dr.resize(m_ary_size);
	init();
}
Board::Board(const Board &x)
	: m_bd_width(x.m_bd_width), m_bd_height(x.m_bd_width)
{
	m_ary_width = x.m_ary_width;
	m_ary_height = x.m_ary_width;
	m_ary_size = x.m_ary_size;
	DR_INDEX = x.DR_INDEX;
	//m_cell.resize(m_ary_size);
	m_cell = x.m_cell;
	m_dist.resize(m_ary_size);
	m_next_gid = x.m_next_gid;
	m_gid = x.m_gid;
	m_parent_ul = x.m_parent_ul;
	m_parent_dr = x.m_parent_dr;
}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), BWALL);	//	for 上下壁
	for(int y = 0; y < m_bd_height; ++y) {
		m_cell[xyToIndex(-1, y)] = WWALL;
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIndex(x, y)] = EMPTY;
	}
	m_rave.resize(m_ary_size);
	fill(m_rave.begin(), m_rave.end(), 0);	//	m_rave[] を 0 に初期化
	m_next_gid = -1;
	fill(m_gid.begin(), m_gid.end(), -1);	//	m_gid[] を -1 に初期化
	fill(m_parent_ul.begin(), m_parent_ul.end(), UNCONNECT);	//	m_parent_ul[] を -1 に初期化
	fill(m_parent_dr.begin(), m_parent_dr.end(), UNCONNECT);	//	m_parent_dr[] を -1 に初期化
	m_parent_ul[UL_INDEX] = UL_INDEX;
	m_parent_dr[DR_INDEX] = DR_INDEX;
}
void Board::print() const {
	cout << "   ";
	for(int x = 0; x < m_bd_width; ++x)
		printf("%c ", 'a'+x);
	cout << endl;
	for(int y = 0; y < m_bd_height; ++y) {
		cout << string(y, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			switch(m_cell[xyToIndex(x, y)]) {
			case EMPTY: cout << "・"; break;
			case BLACK: cout << "●"; break;
			case WHITE: cout << "◯"; break;
			default:
				cout << "？";
			}
		}
		cout << endl;
	}
	cout << endl;
}
bool Board::put_and_check(int x, int y, byte col) {
	int ix = xyToIndex(x, y);
	m_cell[ix] = col;
	check_connected(ix, ix-m_bd_width, col);
	check_connected(ix, ix-m_bd_width+1, col);
	check_connected(ix, ix-1, col);
	check_connected(ix, ix+1, col);
	check_connected(ix, ix+m_bd_width-1, col);
	check_connected(ix, ix+m_bd_width, col);
	if( m_gid[ix] < 0 ) {	//	6近傍に非接続
		m_gid[ix] = ++m_next_gid;
		if( m_min_gid.size() <= m_next_gid ) m_min_gid.resize(m_next_gid+1);
		m_min_gid[m_next_gid] = m_next_gid;
	}
	return false;
}
void Board::check_connected(int ix, int ix2, byte col) {
	if( m_cell[ix2] != col ) return;	//	非接続
	if( m_gid[ix] < 0 ) {				//	ix が接続処理済みではない
		m_gid[ix] = m_gid[ix2];
	} else {		//	ix が接続処理済み → マージ処理
		// それぞれのグループの根（代表ID）を取得
		int root1 = find_root(m_gid[ix]);
		int root2 = find_root(m_gid[ix2]);
		if (root1 != root2) {
			// 根が異なる場合のみマージする
			// 小さい方のIDを根にする
			if (root1 < root2) {
				m_min_gid[root2] = root1;
			} else {
				m_min_gid[root1] = root2;
			}
		}
	}
}
bool Board::put_and_check_uf(int ix, byte col) {	//	return: 着手により上下 or 左右辺が連結されたか？
	//int ix = xyToIndex(x, y);
	m_cell[ix] = col;
	if( col == BLACK ) {
		auto y = indexToY(ix);
		if( y == 0 )
			m_parent_ul[ix] = UL_INDEX;		//	上辺に接続
		else if( y == m_bd_height - 1 )
			m_parent_dr[ix] = DR_INDEX;		//	下辺に接続
	} else {	//	col == WHITE
		auto x = indexToX(ix);
		if( x == 0 )
			m_parent_ul[ix] = UL_INDEX;		//	左辺に接続
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
	return find_root_ul(ix) == UL_INDEX && find_root_dr(ix) == DR_INDEX;
}
void Board::check_connected_uf(int ix, int ix2, byte col) {
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
}
int Board::find_root_ul(int ix) {
	if( m_parent_ul[ix] == ix ) return ix;
	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
	return m_parent_ul[ix] = find_root_ul(m_parent_ul[ix]);
}
int Board::find_root_dr(int ix) {
	if( m_parent_dr[ix] == ix ) return ix;
	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
	return m_parent_dr[ix] = find_root_dr(m_parent_dr[ix]);
}
int Board::find_root(int gid) {
	if( m_min_gid[gid] == gid ) return gid;
	//	再帰的に根を探し、途中のノードを根に直接つなぎ替える（経路圧縮）
	return m_min_gid[gid] = find_root(m_min_gid[gid]);
}
void Board::print_dist() const {
	for(int y = 0; y < m_bd_height; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_dist[xyToIndex(x, y)];
			if( d == DIST_MAX ) cout << "  -1";
			else printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
}
void Board::print_parent() const {
	cout << "parent_ul[]:" << endl;
	for(int y = 0; y < m_bd_height; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_parent_ul[xyToIndex(x, y)];
			printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
	cout << "parent_dr[]:" << endl;
	for(int y = 0; y < m_bd_height; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_parent_dr[xyToIndex(x, y)];
			printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
}
void Board::calc_dist_sub(int ix, int dix, ushort dist, byte col) {
	if( m_cell[dix] == EMPTY && dist + 1 < m_dist[dix] ) {
		m_dist[dix] = dist + 1;
		m_list2.push_back(dix);
	} else if( m_cell[dix] == col && dist < m_dist[dix] ) {
		m_dist[dix] = dist;
		m_list2.push_back(dix);
	}
}
void Board::calc_dist_sub2(int ix, int ix2, int ix3, int dix, ushort dist, byte col) {
	if( m_cell[ix2] == EMPTY && m_cell[ix3] == EMPTY ) {
		if( m_cell[dix] == EMPTY && dist + 1 < m_dist[dix] ) {
			m_dist[dix] = dist + 1;
			m_list2.push_back(dix);
		} else if( m_cell[dix] == col && dist < m_dist[dix] ) {
			m_dist[dix] = dist;
			m_list2.push_back(dix);
		}
	}
}
int Board::calc_vert_dist(bool ex) {
	fill(m_dist.begin(), m_dist.end(), DIST_MAX);
	m_front.clear();
	m_list1.clear();
	for(int x = 0; x < m_bd_width; ++x) {
		int ix = xyToIndex(x, 0);
		switch( m_cell[ix] ) {
		case EMPTY:
			m_dist[ix] = 1;
			m_list1.push_back(ix);
			if( ex && x+1 < m_bd_width && m_cell[ix+1] == EMPTY ) {
				int ix2 = ix + m_ary_width;
				switch( m_cell[ix2] ) {
				case EMPTY:
					m_dist[ix2] = 1;
					m_list1.push_back(ix2);
					break;
				case BLACK:
					m_dist[ix2] = 0;
					m_front.push_back(ix2);
					break;
				}
			}
			break;
		case BLACK:
			m_dist[ix] = 0;
			m_front.push_back(ix);
			break;
		}
	}
	if( m_front.is_empty() ) {
		m_front.swap(m_list1);
		m_list1.clear();
	}
	//print_dist();
	while( !m_front.is_empty() ) {
		m_list2.clear();
		for(auto ix: m_front) {
			auto dist = m_dist[ix];
			calc_dist_sub(ix, ix - m_ary_width, dist, BLACK);
			calc_dist_sub(ix, ix - m_ary_width + 1, dist, BLACK);
			calc_dist_sub(ix, ix - 1, dist, BLACK);
			calc_dist_sub(ix, ix + 1, dist, BLACK);
			calc_dist_sub(ix, ix + m_ary_width - 1, dist, BLACK);
			calc_dist_sub(ix, ix + m_ary_width, dist, BLACK);
			//
			if( ex ) {
				calc_dist_sub2(ix, ix - 1, ix - m_ary_width, ix - m_ary_width - 1, dist, BLACK);
				calc_dist_sub2(ix, ix - m_ary_width, ix - m_ary_width + 1, ix - m_ary_width - m_ary_width + 1, dist, BLACK);
				calc_dist_sub2(ix, ix + 1, ix - m_ary_width + 1, ix - m_ary_width + 2, dist, BLACK);
				calc_dist_sub2(ix, ix - 1, ix + m_ary_width - 1, ix + m_ary_width - 2, dist, BLACK);
				calc_dist_sub2(ix, ix + 1, ix + m_ary_width, ix + m_ary_width + 1, dist, BLACK);
				calc_dist_sub2(ix, ix + m_ary_width - 1, ix + m_ary_width, ix + m_ary_width + m_ary_width - 1, dist, BLACK);
			}
		}
		m_front.swap(m_list2);
		if( !m_list1.is_empty() ) {
			m_front.insert(m_front.end(), m_list1.begin(), m_list1.end());		//	front += list1
			m_list1.clear();
		}
		//print_dist();
	}
	if( ex ) {
		for(int x = 0; x < m_bd_width - 1; ++x) {
			int ix = xyToIndex(x, m_bd_height-1);
			if( m_cell[ix] == EMPTY && m_cell[ix+1] == EMPTY ) {
				m_dist[ix] = min(m_dist[ix], m_dist[ix-m_ary_width+1]);
				m_dist[ix+1] = min(m_dist[ix+1], m_dist[ix-m_ary_width+1]);
			}
		}
	}
	//print_dist();
	ushort mind = DIST_MAX;
	for(int x = 0; x < m_bd_width; ++x) {
		int ix = xyToIndex(x, m_bd_height-1);
		mind = min(mind, m_dist[ix]);
	}
	return mind;
	//auto itr = min_element(&m_cell[xyToIndex(0, m_bd_height-1)], &m_cell[xyToIndex(m_bd_width-1, m_bd_height-1)]);
	//return *itr;
}
int Board::calc_horz_dist(bool ex) {
	fill(m_dist.begin(), m_dist.end(), DIST_MAX);
	m_front.clear();
	m_list1.clear();
	for(int y = 0; y < m_bd_width; ++y) {
		int ix = xyToIndex(0, y);
		switch( m_cell[ix] ) {
		case EMPTY:
			m_dist[ix] = 1;
			m_list1.push_back(ix);
			if( ex && y+1 < m_bd_height && m_cell[ix+m_ary_width] == EMPTY ) {
				int ix2 = ix + 1;
				switch( m_cell[ix2] ) {
				case EMPTY:
					m_dist[ix2] = 1;
					m_list1.push_back(ix2);
					break;
				case WHITE:
					m_dist[ix2] = 0;
					m_front.push_back(ix2);
					break;
				}
			}
			break;
		case WHITE:
			m_dist[ix] = 0;
			m_front.push_back(ix);
			break;
		}
	}
	if( m_front.is_empty() ) {
		m_front.swap(m_list1);
		m_list1.clear();
	}
	//print_dist();
	while( !m_front.is_empty() ) {
		m_list2.clear();
		for(auto ix: m_front) {
			auto dist = m_dist[ix];
			calc_dist_sub(ix, ix - m_ary_width, dist, WHITE);
			calc_dist_sub(ix, ix - m_ary_width + 1, dist, WHITE);
			calc_dist_sub(ix, ix - 1, dist, WHITE);
			calc_dist_sub(ix, ix + 1, dist, WHITE);
			calc_dist_sub(ix, ix + m_ary_width - 1, dist, WHITE);
			calc_dist_sub(ix, ix + m_ary_width, dist, WHITE);
			//
			if( ex ) {
				calc_dist_sub2(ix, ix - 1, ix - m_ary_width, ix - m_ary_width - 1, dist, WHITE);
				calc_dist_sub2(ix, ix - m_ary_width, ix - m_ary_width + 1, ix - m_ary_width - m_ary_width + 1, dist, WHITE);
				calc_dist_sub2(ix, ix + 1, ix - m_ary_width + 1, ix - m_ary_width + 2, dist, WHITE);
				calc_dist_sub2(ix, ix - 1, ix + m_ary_width - 1, ix + m_ary_width - 2, dist, WHITE);
				calc_dist_sub2(ix, ix + 1, ix + m_ary_width, ix + m_ary_width + 1, dist, WHITE);
				calc_dist_sub2(ix, ix + m_ary_width - 1, ix + m_ary_width, ix + m_ary_width + m_ary_width - 1, dist, WHITE);
			}
		}
		m_front.swap(m_list2);
		if( !m_list1.is_empty() ) {
			m_front.insert(m_front.end(), m_list1.begin(), m_list1.end());		//	front += list1
			m_list1.clear();
		}
		//print_dist();
	}
	if( ex ) {
		for(int y = 0; y < m_bd_height - 1; ++y) {
			int ix = xyToIndex(m_bd_width-1, y);
			if( m_cell[ix] == EMPTY && m_cell[ix+m_ary_width] == EMPTY ) {
				m_dist[ix] = min(m_dist[ix], m_dist[ix+m_ary_width-1]);
				m_dist[ix+m_ary_width] = min(m_dist[ix+m_ary_width], m_dist[ix+m_ary_width-1]);
			}
		}
	}
	//print_dist();
	ushort mind = DIST_MAX;
	for(int y = 0; y < m_bd_height; ++y) {
		int ix = xyToIndex(m_bd_width-1, y);
		mind = min(mind, m_dist[ix]);
	}
	return mind;
}
int Board::sel_move_random() {
	g_lst.reserve(m_bd_width*m_bd_height);
	g_lst.clear();
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			g_lst.push_back(ix);
	}
	if( g_lst.is_empty() ) return -1;
	int r = rgen() % g_lst.size();
	return g_lst[r];
}
byte Board::playout_smart(byte next) {
	vector<int> lst;		//	空欄位置リスト
	lst.reserve(m_bd_width*m_bd_height);
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
	shuffle(lst.begin(), lst.end(), rgen);
	for(auto ix: lst) {
		if( put_and_check_uf(ix, next) ) {
			print();
			print_parent();
			return next;
		}
		print();
		print_parent();
		next = (BLACK+WHITE) - next;
	}
	return EMPTY;
}
void Board::playout_to_end(byte next) {
	vector<int> lst;		//	空欄位置リスト
	lst.reserve(m_bd_width*m_bd_height);
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
	shuffle(lst.begin(), lst.end(), rgen);
	for(auto ix: lst) {
		set_color(ix, next);
		next = (BLACK+WHITE) - next;
	}
}
bool Board::playout(byte next) const {		//	return: true for 黒勝ち
	Board bd(*this);
	vector<int> lst;		//	空欄位置リスト
	lst.reserve(m_bd_width*m_bd_height);
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
	shuffle(lst.begin(), lst.end(), rgen);
	for(auto ix: lst) {
		bd.set_color(ix, next);
		next = (BLACK+WHITE) - next;
	}
	return bd.is_vert_connected();
}
bool Board::did_black_win(int ix) {
	return false;
}
void Board::get_empty_list(std::vector<int>& lst) const {
	lst.reserve(m_bd_width*m_bd_height);
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
}
bool Board::playout_rave(byte next) const {		//	return: true for 黒勝ち
	Board bd(*this);
	vector<int> lst;		//	空欄位置リスト
	get_empty_list(lst);
	shuffle(lst.begin(), lst.end(), rgen);
#if 1
	for(int i = 0; i < lst.size(); ++i) {
		//	m_rave[] 最大値の位置を探す
		int max_rave = -9999;
		int max_k = -1;
		for(int k = i; k < lst.size(); ++k) {
			int ix = lst[k];
			if( m_rave[ix] > max_rave ) {
				max_rave = m_rave[ix];
				max_k = k;
			}
		}
		swap(lst[i], lst[max_k]);
		bd.set_color(lst[i], next);
		next = (BLACK+WHITE) - next;
	}
#else
	for(auto ix: lst) {
		bd.set_color(ix, next);
		next = (BLACK+WHITE) - next;
	}
#endif
	auto b = bd.is_vert_connected();
	//	m_rave[] 更新
	byte win, loss;
	if( b ) {
		win = BLACK;
		loss = WHITE;
	} else {
		win = WHITE;
		loss = BLACK;
	}
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( bd.m_cell[ix] == win )
			m_rave[ix] += 1;
		else if( bd.m_cell[ix] == loss )
			m_rave[ix] -= 1;
	}
	//bd.print();
	return b;
}
bool Board::is_vert_connected() {
	m_list1.resize(m_cell.size());
	fill(m_list1.begin(), m_list1.end(), 0);
	int ix0 = xyToIndex(0, 0);
	int ix = xyToIndex(m_bd_width, 0);
	while( --ix >= ix0 ) {
		if( m_cell[ix] == BLACK ) {
			m_list1[ix] = 1;
			if( is_vert_connected_sub(ix) )
				return true;
		}
	}
	return false;
}
bool Board::is_vert_connected_sub(int ix) {
	const int ixd = xyToIndex(0, m_bd_height-1);
	int ix2 = ix + m_ary_width;
	if( m_cell[ix2] == BLACK && m_list1[ix2] == 0 ) {
		if( ix2 >= ixd ) return true;
		m_list1[ix2] = 1;
		if( is_vert_connected_sub(ix2) )
			return true;
	}
	ix2 = ix + m_ary_width - 1;
	if( m_cell[ix2] == BLACK && m_list1[ix2] == 0 ) {
		if( ix2 >= ixd ) return true;
		m_list1[ix2] = 1;
		if( is_vert_connected_sub(ix2) )
			return true;
	}
	ix2 = ix + 1;
	if( m_cell[ix2] == BLACK && m_list1[ix2] == 0 ) {
		m_list1[ix2] = 1;
		if( is_vert_connected_sub(ix2) )
			return true;
	}
	ix2 = ix - 1;
	if( m_cell[ix2] == BLACK && m_list1[ix2] == 0 ) {
		m_list1[ix2] = 1;
		if( is_vert_connected_sub(ix2) )
			return true;
	}
	ix2 = ix - m_ary_width + 1;
	if( m_cell[ix2] == BLACK && m_list1[ix2] == 0 ) {
		m_list1[ix2] = 1;
		if( is_vert_connected_sub(ix2) )
			return true;
	}
	ix2 = ix - m_ary_width;
	if( m_cell[ix2] == BLACK && m_list1[ix2] == 0 ) {
		m_list1[ix2] = 1;
		if( is_vert_connected_sub(ix2) )
			return true;
	}
	return false;
}
bool Board::playout_old(byte next) const {		//	return: true for 黒勝ち
	Board bd(*this);
	int dv, dh;
	for(;;) {
		int ix = bd.sel_move_random();
		if( ix < 0 ) break;
		bd.set_color(ix, next);
		if( next == BLACK ) {
			dv = bd.calc_vert_dist(true);
			if( dv == 0 ) {
				return true;		//	黒勝ち
			}
		} else {
			dh = bd.calc_horz_dist(true);
			if( dh == 0 ) {
				return false;		//	白勝ち
			}
		}
		next = (BLACK + WHITE) - next;
	}
	//print();
	dv = bd.calc_vert_dist(true);
	return dv == 0;
	//	ここには来ないはず？
	assert( 0 );
	return true;
}
double Board::estimate_win_rate_PMC(byte next, int N) const {	//	return: 次の手番勝率
	int black_won = 0;
	for(int i = 0;i < N; ++i) {
		if( playout(next) )
			++black_won;
	}
	if( next == BLACK )
		return (double)black_won / N;
	else
		return (double)(N - black_won) / N;
}
int Board::sel_move_PMC(byte next) {
	const int N_PLAYOUT = 1000;
	byte n2 = (BLACK+WHITE) - next;
	int best_ix = -1;
	double best_ev = -1;
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			auto ev = 1.0 - estimate_win_rate_PMC(n2, N_PLAYOUT);
			if( ev > best_ev ) {
				best_ev = ev;
				best_ix = ix;
			}
			m_cell[ix] = EMPTY;
		}
	}
	return best_ix;
}
int Board::eval() {
	return calc_horz_dist() - calc_vert_dist();
	//if( next == BLACK )
	//	return calc_horz_dist() - calc_vert_dist() + 1;
	//else
	//	return calc_vert_dist() - calc_horz_dist() - 1;
}
int Board::alpha_beta_black(int alpha, int beta, int depth) {
	if( depth == 0 )
		return eval();
	for(int y = 0; y < m_bd_height; ++y) {
		for(int x = 0; x < m_bd_width; ++x) {
		}
	}
}
int Board::alpha_beta_white(int alpha, int beta, int depth) {
	return 0;
}
int Board::black_turn(int depth) {
	if( depth != 0 ) {
		int max_ev = INT_MIN;
		for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
			if( m_cell[ix] == EMPTY ) {
				m_cell[ix] = BLACK;
				auto ev = white_turn(depth-1);
				max_ev = max(max_ev, ev);
				m_cell[ix] = EMPTY;
			}
		}
		if( max_ev != INT_MIN )
			return max_ev;
	}
	return eval();
}
int Board::white_turn(int depth) {
	if( depth != 0 ) {
		int min_ev = SHRT_MAX;
		for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_height-1); ++ix) {
			if( m_cell[ix] == EMPTY ) {
				m_cell[ix] = WHITE;
				auto ev = black_turn(depth-1);
				min_ev = min(min_ev, ev);
				m_cell[ix] = EMPTY;
			}
		}
		if( min_ev != SHRT_MAX )
			return min_ev;
	}
	return eval();
}
//
bool dfs_black_win(Board& bd, byte next) {		//	双方最善で黒が勝つか？
	bool put = false;
	for(int ix = bd.xyToIndex(0, 0); ix <= bd.xyToIndex(bd.m_bd_width-1, bd.bd_height()-1); ++ix) {
		if( bd.m_cell[ix] == EMPTY ) {
			bd.m_cell[ix] = next;
			auto b = dfs_black_win(bd, (BLACK+WHITE)-next);
			bd.m_cell[ix] = EMPTY;
			if( (next == BLACK && b) || (next == WHITE && !b) )
				return b;	//	勝ちがあれば、確定
			put = true;
		}
	}
	if( put ) {		//	全ての空欄を試したが勝ち無し
		return next == BLACK ? false : true;
	}
	//	空欄無しの場合
	return bd.is_vert_connected();
}
