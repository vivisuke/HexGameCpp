#include "HNetBoard.h"
#include <iostream>
#include <string>
#include <deque>
#include <algorithm> 

using namespace std;

#define		is_empty()		empty()

HNetBoard::HNetBoard(int width)
	: m_width(width)
	, TOP_NODE_IX(width*width)
	, BOTTOM_NODE_IX(width*width+1)
	, LEFT_NODE_IX(width*width+2)
	, RIGHT_NODE_IX(width*width+3)
{
	m_node.resize(width*width+4);	//	4 for 上下左右仮想ノード
	m_node[TOP_NODE_IX].m_color = BLACK;
	m_node[BOTTOM_NODE_IX].m_color = BLACK;
	m_node[LEFT_NODE_IX].m_color = WHITE;
	m_node[RIGHT_NODE_IX].m_color = WHITE;
	m_dist.resize(width*width+4);
	init();
}

void HNetBoard::init() {
	for(auto &node: m_node) {
		//node.m_is_connected.resize(m_node.size());
		node.m_network_black.resize(m_node.size());
		node.m_network_white.resize(m_node.size());
		//for(int i = 0; i != node.m_network_black.size(); ++i) {
		//	node.m_network_black[i] = 0;	//	非連結
		//	node.m_network_white[i] = 0;	//	非連結
		//}
		fill(node.m_network_black.begin(), node.m_network_black.end(), 0);
		fill(node.m_network_white.begin(), node.m_network_white.end(), 0);
	}
	for(int y = 0; y != m_width; ++y) {
		for(int x = 0; x != m_width; ++x) {
			int ix = xyToIX(x, y);
			//	６近傍エッジ
			if( x > 0 ) {
				m_node[ix].m_network_black[ix-1] = 1;
				m_node[ix].m_network_white[ix-1] = 1;
				if( y < m_width - 1) {
					m_node[ix].m_network_black[ix+m_width-1] = 1;
					m_node[ix].m_network_white[ix+m_width-1] = 1;
				}
			}
			if( x < m_width - 1) {
				m_node[ix].m_network_black[ix+1] = 1;
				m_node[ix].m_network_white[ix+1] = 1;
				if( y > 0 ) {
					m_node[ix].m_network_black[ix-m_width+1] = 1;
					m_node[ix].m_network_white[ix-m_width+1] = 1;
				}
			}
			if( y > 0 ) {
				m_node[ix].m_network_black[ix-m_width] = 1;
				m_node[ix].m_network_white[ix-m_width] = 1;
			}
			if( y < m_width - 1) {
				m_node[ix].m_network_black[ix+m_width] = 1;
				m_node[ix].m_network_white[ix+m_width] = 1;
			}
			//
			if( y == 0 ) {	//	上辺仮想ノードエッジ
				m_node[ix].m_network_black[TOP_NODE_IX] = 1;
				m_node[TOP_NODE_IX].m_network_black[ix] = 1;
			} else if( y == m_width - 1 ) {	//	下辺仮想ノードエッジ
				m_node[ix].m_network_black[BOTTOM_NODE_IX] = 1;
				m_node[BOTTOM_NODE_IX].m_network_black[ix] = 1;
			}
			//
			if( x == 0 ) {	//	左辺仮想ノードエッジ
				m_node[ix].m_network_white[LEFT_NODE_IX] = 1;
				m_node[LEFT_NODE_IX].m_network_white[ix] = 1;
			} else if( x == m_width - 1 ) {	//	右辺仮想ノードエッジ
				m_node[ix].m_network_white[RIGHT_NODE_IX] = 1;
				m_node[RIGHT_NODE_IX].m_network_white[ix] = 1;
			}
		}
	}
}
const string color_str[] = {"・", "●", "◯"};
const string vnode_str[] = {"T", "B", "L", "R"};
const char *vnode_chars = "TBLR";
string HNetBoard::ixToCoord(int ix) const {
	string txt(2, ' ');
	if( ix < TOP_NODE_IX) {
		txt[0] = 'a'+(ix%m_width);
		txt[1] = '1' + (ix / m_width);
	} else {
		txt[1] = vnode_chars[ix - TOP_NODE_IX];
	}
	return txt;
}
void HNetBoard::print() const {
	cout << "BLACK network:" << endl;
	for(int i = 0; i != m_node.size()-2; ++i) {
		const auto &node = m_node[i];
		cout << " " << ixToCoord(i);
		cout << ": " << color_str[node.m_color];
		for(int i = 0; i != node.m_network_black.size(); ++i) {
			if( node.m_network_black[i] != 0 )
				cout << " " << ixToCoord(i);
		}
		cout << endl;
	}
	cout << endl;
	cout << "WHITE network:" << endl;
	for(int i = 0; i != m_node.size(); ++i) {
		if( i >= TOP_NODE_IX && i <= BOTTOM_NODE_IX ) continue;
		const auto &node = m_node[i];
		cout << " " << ixToCoord(i);
		cout << ": " << color_str[node.m_color];
		for(int i = 0; i != node.m_network_white.size(); ++i) {
			if( node.m_network_white[i] != 0 )
				cout << " " << ixToCoord(i);
		}
		cout << endl;
	}
	cout << endl;
}
void HNetBoard::print_dist() const {
	for(int y = 0; y < m_width; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_width; ++x) {
			auto d = m_dist[xyToIX(x, y)];
			//if( d == DIST_MAX ) cout << "  -1";
			//else
			printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
}
void HNetBoard::update_network(int ix, Color col) {
	if( col == BLACK ) {
		vector<short> lst;
		for (int ix2 = 0; ix2 != m_node[ix].m_network_black.size(); ++ix2) {
			if( m_node[ix].m_network_black[ix2] != 0 ) {	//	距離１
				lst.push_back(ix2);
				m_node[ix2].m_network_black[ix] = 0;		//	切断
				m_node[ix].m_network_black[ix2] = 0;		//	切断
			}
		}
		for(int i = 0; i < lst.size() - 1; ++i) {
			for(int k = i + 1; k < lst.size(); ++k) {
				m_node[lst[i]].m_network_black[lst[k]] = 1;		//	距離１
				m_node[lst[k]].m_network_black[lst[i]] = 1;		//	距離１
			}
		}
		for (int ix2 = 0; ix2 != m_node[ix].m_network_white.size(); ++ix2) {
			if( m_node[ix].m_network_white[ix2] != 0 ) {	//	距離１
				m_node[ix2].m_network_white[ix] = 0;		//	切断
				m_node[ix].m_network_white[ix2] = 0;		//	切断
			}
		}
	} else {
	}
}
int HNetBoard::calc_vert_distance() const {
	fill(m_dist.begin(), m_dist.end(), -1);		//	未探索ノードを -1 で初期化
	m_dist[TOP_NODE_IX] = 0;
	deque<short> que;
	que.push_back(TOP_NODE_IX);
	while( !que.is_empty() ) {
		auto ix = que.front();
		que.pop_front();
		auto d1 = m_dist[ix] + 1;
		//for(auto ix2: m_node[ix].m_network_black) {
		for (int ix2 = 0; ix2 != m_node[ix].m_network_black.size(); ++ix2) {
			if( m_node[ix].m_network_black[ix2] != 0 && m_dist[ix2] < 0 ) {	//	距離１＆未探索の場合
				m_dist[ix2] = d1;
				que.push_back(ix2);
			}
		}
	}
	return m_dist[BOTTOM_NODE_IX] - 1;
}
