#include <iostream>
#include <string>
#include "NEBoard.h"

using namespace std;

NEBoard::NEBoard(int width)
	: m_width(width)
	, TOP_NODE_IX(width*width)
	, LEFT_NODE_IX(width*width+1)
	, RIGHT_NODE_IX(width*width+2)
	, BOTTOM_NODE_IX(width*width+3)
{
	m_cell.resize(width*width + 4);		//	+4 for 上下左右仮想ノード
	init_nx_list();
	init();
}
void NEBoard::init_nx_list() {
	for(int y = 0; y != m_width; ++y) {
		for(int x = 0; x != m_width; ++x) {
			int ix = xyToIX(x, y);
			//	６近傍エッジ
			if( x > 0 ) {
				m_cell[ix].m_nx_list.push_back(ix-1);
				if( y < m_width - 1)
					m_cell[ix].m_nx_list.push_back(ix+m_width-1);
			}
			if( x < m_width - 1) {
				m_cell[ix].m_nx_list.push_back(ix+1);
				if( y > 0 )
					m_cell[ix].m_nx_list.push_back(ix-m_width+1);
			}
			if( y > 0 )
				m_cell[ix].m_nx_list.push_back(ix-m_width);
			if( y < m_width - 1)
				m_cell[ix].m_nx_list.push_back(ix+m_width);
			//
			if( y == 0 ) {	//	上辺仮想ノードエッジ
				m_cell[ix].m_nx_list.push_back(TOP_NODE_IX);
				m_cell[TOP_NODE_IX].m_nx_list.push_back(ix);
			} else if( y == m_width - 1 ) {	//	下辺仮想ノードエッジ
				m_cell[ix].m_nx_list.push_back(BOTTOM_NODE_IX);
				m_cell[BOTTOM_NODE_IX].m_nx_list.push_back(ix);
			}
			//
			if( x == 0 ) {	//	左辺仮想ノードエッジ
				m_cell[ix].m_nx_list.push_back(LEFT_NODE_IX);
				m_cell[LEFT_NODE_IX].m_nx_list.push_back(ix);
			} else if( x == m_width - 1 ) {	//	右辺仮想ノードエッジ
				m_cell[ix].m_nx_list.push_back(RIGHT_NODE_IX);
				m_cell[RIGHT_NODE_IX].m_nx_list.push_back(ix);
			}
		}
	}
}
void NEBoard::init() {
	for(int y = 0; y != m_width; ++y) {
		for(int x = 0; x != m_width; ++x) {
			m_cell[xyToIX(x, y)].m_color = EMPTY;
		}
	}
}
//string col_str[] = {"E", "B", "W"};
string col_str[] = {"・", "●", "◯"};
void NEBoard::print_node_edge() const {
	for(int i = 0; i != m_cell.size(); ++i) {
		const auto node = m_cell[i];
		printf("%3d: %s", i, &col_str[node.m_color][0]);
		//cout << i << ": " << col_str[node.m_color];
		for(int k = 0; k != node.m_nx_list.size(); ++k)
			cout << " " << node.m_nx_list[k];
		cout << endl;
	}
}
