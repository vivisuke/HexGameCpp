#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <assert.h>
#include "Board.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // î‘ï∫Ç¬Ç´ÇPéüå≥îzóÒÉÅÉÇÉäämï€
    init();  // î’ñ èâä˙âª

}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), WALL);	//	for è„â∫ï«
	for(int y = 0; y < m_bd_width; ++y) {
		m_cell[xyToIndex(-1, y)] = WALL;		//	ç∂âEï«
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIndex(x, y)] = EMPTY;
	}
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
			switch(m_cell[xyToIndex(x, y)]) {
			case EMPTY: cout << "ÅE"; break;
			case BLACK: cout << "Åú"; break;
			case WHITE: cout << "Å¸"; break;
			default:
				cout << "ÅH";
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
		if( is_vert_connected_DFS(xyToIndex(x, 0)) )		//	ê[Ç≥óDêÊíTçı
			return true;	//	è„â∫òAåãåoòHÇî≠å©ÇµÇΩèÍçá
	}
	return false;
}
bool Board::is_vert_connected_DFS(int ix) const {
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	çïÇ≈Ç»Ç¢ or íTçıçœÇ›
		return false;
	if( ix >= xyToIndex(0, m_bd_width-1) )		//	â∫ï”Ç…ìûíBÇµÇΩèÍçá
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
		if( is_horz_connected_DFS(xyToIndex(0, y)) )		//	ê[Ç≥óDêÊíTçı
			return true;	//	è„â∫òAåãåoòHÇî≠å©ÇµÇΩèÍçá
	}
	return false;
}
bool Board::is_horz_connected_DFS(int ix) const {
	if( m_cell[ix] != WHITE || m_connected[ix] != UNSEARCHED )	//	çïÇ≈Ç»Ç¢ or íTçıçœÇ›
		return false;
	if( ixToX(ix) == m_bd_width - 1 )		//	âEï”Ç…ìûíBÇµÇΩèÍçá
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
void Board::random_playout(byte next) {
	vector<int> lst;
	get_empty_indexes(lst);
	shuffle(lst.begin(), lst.end(), rgen);
	for(int ix: lst) {
		set_color(ix, next);
		next = (BLACK+WHITE) - next;
	}
};
bool Board::playout_to_full(byte next) {
	random_playout(next);
	if( next == BLACK )
		return is_vert_connected();
	else
		return !is_vert_connected();
}
bool Board::playout_to_win(byte next) {
	auto n0 = next;
	vector<int> lst;
	get_empty_indexes(lst);
	shuffle(lst.begin(), lst.end(), rgen);
	for(int ix: lst) {
		set_color(ix, next);
		if( next == BLACK && is_vert_connected() ||
			next == WHITE && is_horz_connected() )
		{
			break;
		}
		next = (BLACK+WHITE) - next;
	}
	return next == n0;
};
