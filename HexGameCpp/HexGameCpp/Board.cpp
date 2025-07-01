#include "Board.h"

Board::Board(int bd_width)
	: m_bd_width(bd_width)
{
	m_ary_width = m_bd_width + 1;
	m_ary_height = m_bd_width + 2;
	m_ary_size = m_ary_width * m_ary_height;
	m_board.resize(m_ary_size);
	init();
}
void Board::init() {

}
