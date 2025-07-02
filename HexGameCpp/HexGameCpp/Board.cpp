#include <iostream>
#include <string>
#include <algorithm>
#include "Board.h"

using namespace std;

#define		is_empty()	empty()

Board::Board(int bd_width)
	: m_bd_width(bd_width), m_bd_height(bd_width)
{
	m_ary_width = m_bd_width + 1;
	m_ary_height = m_bd_width + 2;
	m_ary_size = m_ary_width * m_ary_height;
	m_cell.resize(m_ary_size);
	m_dist.resize(m_ary_size);
	init();
}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), BWALL);	//	for 上下壁
	for(int y = 0; y < m_bd_height; ++y) {
		m_cell[xyToIndex(-1, y)] = WWALL;
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIndex(x, y)] = EMPTY;
	}
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
void Board::print_dist() const {
	for(int y = 0; y < m_bd_height; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_dist[xyToIndex(x, y)];
			if( d == USHORT_MAX ) cout << "  -1";
			else printf("%4d", d);
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
int Board::calc_vert_dist() {
	fill(m_dist.begin(), m_dist.end(), USHORT_MAX);
	m_front.clear();
	m_list1.clear();
	for(int x = 0; x < m_bd_width; ++x) {
		int ix = xyToIndex(x, 0);
		switch( m_cell[ix] ) {
		case EMPTY:
			m_dist[ix] = 1;
			m_list1.push_back(ix);
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
		}
		m_front.swap(m_list2);
		if( !m_list1.is_empty() ) {
			m_front.insert(m_front.end(), m_list1.begin(), m_list1.end());		//	front += list1
			m_list1.clear();
		}
		//print_dist();
	}
	ushort mind = USHORT_MAX;
	for(int x = 0; x < m_bd_width; ++x) {
		int ix = xyToIndex(x, m_bd_height-1);
		mind = min(mind, m_dist[ix]);
	}
	return mind;
	//auto itr = min_element(&m_cell[xyToIndex(0, m_bd_height-1)], &m_cell[xyToIndex(m_bd_width-1, m_bd_height-1)]);
	//return *itr;
}
int Board::calc_horz_dist() {
	fill(m_dist.begin(), m_dist.end(), USHORT_MAX);
	m_front.clear();
	m_list1.clear();
	for(int y = 0; y < m_bd_width; ++y) {
		int ix = xyToIndex(0, y);
		switch( m_cell[ix] ) {
		case EMPTY:
			m_dist[ix] = 1;
			m_list1.push_back(ix);
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
		}
		m_front.swap(m_list2);
		if( !m_list1.is_empty() ) {
			m_front.insert(m_front.end(), m_list1.begin(), m_list1.end());		//	front += list1
			m_list1.clear();
		}
		//print_dist();
	}
	ushort mind = USHORT_MAX;
	for(int y = 0; y < m_bd_width; ++y) {
		int ix = xyToIndex(m_bd_width-1, y);
		mind = min(mind, m_dist[ix]);
	}
	return mind;
}
