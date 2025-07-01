#include <iostream>
#include <string>
#include <algorithm>
#include "Board.h"

using namespace std;

Board::Board(int bd_width)
	: m_bd_width(bd_width), m_bd_height(bd_width)
{
	m_ary_width = m_bd_width + 1;
	m_ary_height = m_bd_width + 2;
	m_ary_size = m_ary_width * m_ary_height;
	m_board.resize(m_ary_size);
	init();
}
void Board::init() {
	fill(m_board.begin(), m_board.end(), BWALL);	//	for 上下壁
	for(int y = 0; y < m_bd_height; ++y) {
		m_board[xyToIndex(-1, y)] = WWALL;
		for(int x = 0; x < m_bd_width; ++x)
			m_board[xyToIndex(x, y)] = EMPTY;
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
			switch(m_board[xyToIndex(x, y)]) {
			case EMPTY: cout << "・"; break;
			case BLACK: cout << "●"; break;
			case WHITE: cout << "◯"; break;
			default:
				cout << "？";
			}
		}
		cout << endl;
	}
}
