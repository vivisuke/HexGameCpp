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
	fill(m_board.begin(), m_board.end(), BWALL);	//	for è„â∫ï«
	for(int y = 0; y < m_bd_height; ++y) {
		m_board[xyToIndex(-1, y)] = WWALL;
		for(int x = 0; x < m_bd_width; ++x)
			m_board[xyToIndex(x, y)] = EMPTY;
	}
}
