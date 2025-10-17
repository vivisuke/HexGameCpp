#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <queue>
#include <assert.h>
#include "Board.h"

using namespace std;

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // �ԕ����P�����z�񃁃����m��
	m_parent_ul.resize(m_ary_size);
    init();  // �Ֆʏ�����
}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), WALL);	//	for �㉺��
	for(int y = 0; y < m_bd_width; ++y) {
		m_cell[xyToIX(-1, y)] = WALL;		//	���E��
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIX(x, y)] = EMPTY;
	}
	for(int i = 0; i != m_parent_ul.size(); ++i)
		m_parent_ul[i] = i;
	m_parent_ul[TL_INDEX] = TL_INDEX;
	m_parent_ul[BT_INDEX] = BT_INDEX;
	m_parent_ul[RT_INDEX] = RT_INDEX;
	m_uf_stack.clear();
	m_uf_stack.push_back(0);		//	undo ����؂�
	m_last_put_ix = 0;
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
			case EMPTY: cout << "�E"; break;
			case BLACK: cout << (ix==m_last_put_ix?"��":"��"); break;
			case WHITE: cout << (ix==m_last_put_ix?"��":"��"); break;
			default:
				cout << "�H";
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
		if( is_vert_connected_DFS(xyToIX(x, 0)) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	return false;
}
bool Board::is_vert_connected_DFS(int ix) const {
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	���łȂ� or �T���ς�
		return false;
	if( ix >= xyToIX(0, m_bd_width-1) )		//	���ӂɓ��B�����ꍇ
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
		if( is_horz_connected_DFS(xyToIX(0, y)) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	return false;
}
bool Board::is_horz_connected_DFS(int ix) const {
	if( m_cell[ix] != WHITE || m_connected[ix] != UNSEARCHED )	//	���łȂ� or �T���ς�
		return false;
	if( ixToX(ix) == m_bd_width - 1 )		//	�E�ӂɓ��B�����ꍇ
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
		if( is_vert_connected_v_DFS(ix, ix-m_ary_width, ix-m_ary_width+1) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	for(int x = 0; x < m_bd_width; ++x) {
		if( is_vert_connected_v_DFS(xyToIX(x, 0)) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	return false;
}
/*
                  -2W+1
        -W-1 -W   -W+1 -W+2
        -1   0    +1
   +W-2 +W-1 +W   +W+1
        +2W-1
*/
bool Board::is_vert_connected_v_DFS(int ix, int mx1, int mx2) const {
	if (mx1 >= 0 && (m_cell[mx1] != EMPTY || m_cell[mx2] != EMPTY))	//	�������ʒu���󗓂Ŗ���
		return false;
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	���łȂ� or �T���ς�
		return false;
	if( ixToY(ix) == m_bd_width-2 && m_cell[ix+m_ary_width-1] == EMPTY && m_cell[ix+m_ary_width] == EMPTY )
		return true;						//	���ӂɌ������A��
	if( ix >= xyToIX(0, m_bd_width-1) )		//	���ӂɓ��B�����ꍇ
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
	if( is_vert_connected_v_DFS(ix + 2*W - 1, ix + W - 1, ix + W) ) return true;
	if( is_vert_connected_v_DFS(ix + W - 2, ix + W - 1, ix - 1) ) return true;
	if( is_vert_connected_v_DFS(ix + W + 1, ix + 1, ix + W) ) return true;
	if( is_vert_connected_v_DFS(ix - W + 2, ix - W + 1, ix + 1) ) return true;
	if( is_vert_connected_v_DFS(ix - W - 1, ix - W, ix - 1) ) return true;
	if( is_vert_connected_v_DFS(ix - 2*W + 1, ix - W, ix - W + 1) ) return true;
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
		if( is_horz_connected_v_DFS(ix, ix-1, ix+m_ary_width-1) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	for(int y = 0; y < m_bd_width; ++y) {
		if( is_horz_connected_v_DFS(xyToIX(0, y)) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	return false;
}
bool Board::is_horz_connected_v_DFS(int ix, int mx1, int mx2) const {
	if (mx1 >= 0 && (m_cell[mx1] != EMPTY || m_cell[mx2] != EMPTY))	//	�������ʒu���󗓂Ŗ���
		return false;
	if( m_cell[ix] != WHITE || m_connected[ix] != UNSEARCHED )	//	���łȂ� or �T���ς�
		return false;
	int x = ixToX(ix);
	if( x == m_bd_width-2 && m_cell[ix+1] == EMPTY && m_cell[ix-m_ary_width+1] == EMPTY )
		return true;						//	�E�ӂɌ������A��
	if( x == m_bd_width-1 )		//	�E�ӂɓ��B�����ꍇ
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
bool Board::union_find(int ix, Color col) {	//	return: ����ɂ��㉺ or ���E�ӂ��A�����ꂽ���H
	if( col == BLACK ) {
		int x = ixToY(ix);
		if( x == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	��ӂɐڑ�
		} else if( x == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = BT_INDEX;		//	���ӂɐڑ�
		}
	} else {	//	col == WHITE
		int y = ixToX(ix);
		if( y == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	���ӂɐڑ�
		} else if( y == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = RT_INDEX;		//	�E�ӂɐڑ�
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
	if( m_cell[ix2] != col ) return;	//	��ڑ�
	if( m_parent_ul[ix] == ix ) {				//	ix ���ڑ������ς݂ł͂Ȃ�
		m_uf_stack.push_back(m_parent_ul[ix]);
		m_uf_stack.push_back(ix);
		m_parent_ul[ix] = find_root_ul(ix2);
	} else {		//	ix ���ڑ������ς� �� �}�[�W����
		// ���ꂼ��̃O���[�v�̍��i��\ID�j���擾
		int root1 = find_root_ul(ix);
		int root2 = find_root_ul(ix2);
		if (root1 != root2) {
			// �����قȂ�ꍇ�̂݃}�[�W�A����������ID�����ɂ���
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
bool Board::union_find_v(int ix, Color col) {	//	return: ����ɂ��㉺ or ���E�ӂ��A�����ꂽ���H
	if( col == BLACK ) {
		int x = ixToY(ix);
		if( x == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	��ӂɐڑ�
		} else if( x == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = BT_INDEX;		//	���ӂɐڑ�
		}
	} else {	//	col == WHITE
		int y = ixToX(ix);
		if( y == 0 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = TL_INDEX;		//	���ӂɐڑ�
		} else if( y == m_bd_width - 1 ) {
			m_uf_stack.push_back(m_parent_ul[ix]);
			m_uf_stack.push_back(ix);
			m_parent_ul[ix] = RT_INDEX;		//	�E�ӂɐڑ�
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
	//	�ċA�I�ɍ���T���A�r���̃m�[�h�����ɒ��ڂȂ��ւ���i�o�H���k�j
	//return m_parent_ul[ix] = find_root_ul(m_parent_ul[ix]);
	auto root = find_root_ul(m_parent_ul[ix]);
	if( root != m_parent_ul[ix] ) {
		m_uf_stack.push_back(m_parent_ul[ix]);
		m_uf_stack.push_back(ix);
		m_parent_ul[ix] = root;
	}
	return root;
}
