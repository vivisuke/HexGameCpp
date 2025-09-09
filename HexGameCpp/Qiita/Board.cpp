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
std::mt19937_64 rgen64(rd());		// 64�r�b�g��

#define		is_empty()	empty()

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // �ԕ����P�����z�񃁃����m��
	m_parent_ul.resize(m_ary_size);
	//m_parent_dr.resize(m_ary_size);
    init();  // �Ֆʏ�����

}
void Board::init() {
	fill(m_cell.begin(), m_cell.end(), WALL);	//	for �㉺��
	for(int y = 0; y < m_bd_width; ++y) {
		m_cell[xyToIndex(-1, y)] = WALL;		//	���E��
		for(int x = 0; x < m_bd_width; ++x)
			m_cell[xyToIndex(x, y)] = EMPTY;
	}
	for(int i = 0; i != m_parent_ul.size(); ++i)
		m_parent_ul[i] = i;
	m_parent_ul[TL_INDEX] = TL_INDEX;
	m_parent_ul[BT_INDEX] = BT_INDEX;
	m_parent_ul[RT_INDEX] = RT_INDEX;
	m_last_put_ix = 0;
}
Board& Board::operator=(const Board& s) {
	m_cell = s.m_cell;

	return *this;
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
void Board::print_dist() const {
	for(int y = 0; y < m_bd_width; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_dist[xyToIndex(x, y)];
			if( d == UNCONNECT) cout << "  -1";
			else printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
}
bool Board::is_vert_connected() const {
	m_connected.resize(m_ary_size);
	fill(m_connected.begin(), m_connected.end(), UNSEARCHED);
	for(int x = 0; x < m_bd_width; ++x) {
		if( is_vert_connected_DFS(xyToIndex(x, 0)) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	return false;
}
bool Board::is_vert_connected_DFS(int ix) const {
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	���łȂ� or �T���ς�
		return false;
	if( ix >= xyToIndex(0, m_bd_width-1) )		//	���ӂɓ��B�����ꍇ
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
		if( is_horz_connected_DFS(xyToIndex(0, y)) )		//	�[���D��T��
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
//int Board::calc_vert_dist(bool bridge) const {
//	return calc_dist(true, bridge);
//}
//int Board::calc_horz_dist(bool bridge) const {
//	return calc_dist(false, bridge);
//}
//	
int Board::calc_dist(bool vertical, bool bridge, bool rev) const
{
	const Color own_color   = vertical ? BLACK : WHITE;
	const Color opp_color = vertical ? WHITE : BLACK;
	//vector<int> dist(m_ary_size, UNCONNECT);	// �e�Z���܂ł̍ŒZ�������i�[����z��BUNCONNECT�ŏ������B
	m_dist.resize(m_ary_size);
	fill(m_dist.begin(), m_dist.end(), UNCONNECT);
	deque<int> q;	// 0-1 BFS �̂��߂̗��[�L���[
	// 1. �X�^�[�g�n�_�i��Ӂj���L���[�ɒǉ�
	for (int i = 0; i < m_bd_width; ++i) {
		int ix = vertical ? xyToIndex(i, 0) : xyToIndex(0, i);
		if (m_cell[ix] == BLACK) {
			m_dist[ix] = 0;
			q.push_front(ix); // �R�X�g0�Ȃ̂Ő擪�ɒǉ�
		} else if(m_cell[ix] == EMPTY) {
			m_dist[ix] = 1;
			q.push_back(ix);  // �R�X�g1�Ȃ̂Ŗ����ɒǉ�
		}
	}
	if( bridge ) {
		bool is_emp0 = m_cell[xyToIndex(0, 0)] == EMPTY;	//	�󗓂��H
		for (int i = 1; i < m_bd_width; ++i) {
			int ix = vertical ? xyToIndex(i, 0) : xyToIndex(0, i);
			bool is_emp = m_cell[ix] == EMPTY;	//	�󗓂��H
			if( is_emp0 && is_emp ) {
				int ix2 = ix + (vertical ? m_ary_width - 1 : -m_ary_width + 1);
				if (m_cell[ix2] == own_color) {
					m_dist[ix2] = 0;
					q.push_front(ix2); // �R�X�g0�Ȃ̂Ő擪�ɒǉ�
				} else if (m_cell[ix2] == EMPTY) {
					m_dist[ix2] = 1;
					q.push_back(ix2);  // �R�X�g1�Ȃ̂Ŗ����ɒǉ�
				}
			}
			is_emp0 = is_emp;
		}
	}
    if (q.empty()) {    // ������ӂɗL���ȃ}�X������Ȃ���ΘA���s�\
        return UNCONNECT;
    }
	const int offsets[] = {	// �אڃZ���̃C���f�b�N�X����
		-1, +1, -m_ary_width, +m_ary_width, -m_ary_width + 1, +m_ary_width - 1
	};
	int min_dist = UNCONNECT;
	while (!q.empty()) {	// 2. 0-1 BFS ���[�v
		int cur_ix = q.front();
		q.pop_front();
		int current_dist = m_dist[cur_ix];
	
		// ���ɉ��ӂւ̂��Z���p�X���������Ă���ꍇ�A�}����
		if (min_dist != UNCONNECT && current_dist >= min_dist) {
			continue;
		}
		// �S�[���i���� or �E�Ӂj�ɓ��B�������`�F�b�N
#if 0
		if (vertical && ixToY(cur_ix) == m_bd_width - 1 || !vertical && ixToX(cur_ix) == m_bd_width - 1) {
			//if (min_dist == UNCONNECT || current_dist < min_dist) {
			//	min_dist = current_dist;
			//}
            return current_dist;
		}
#endif
		if( vertical ) {
			int y = ixToY(cur_ix);
			if( y == m_bd_width - 1 ||
				bridge && y == m_bd_width - 2 &&
				m_cell[cur_ix+m_ary_width-1] == EMPTY && m_cell[cur_ix+m_ary_width] == EMPTY )
			{
	            return current_dist;
			}
		} else {
			int x = ixToX(cur_ix);
			if( x == m_bd_width - 1 ||
				bridge && x == m_bd_width - 2 &&
				m_cell[cur_ix-m_ary_width+1] == EMPTY && m_cell[cur_ix+1] == EMPTY )
			{
	            return current_dist;
			}
		}
		for (int offset : offsets) {	// �אڃZ����T��
			int next_ix = cur_ix + offset;
			// �ǁE�ՊO�E���΂͖���
			if (m_cell[next_ix] == WALL || m_cell[next_ix] == opp_color) {
				continue;
			}
			// �ړ��R�X�g���v�Z (��}�X�Ȃ�1, ���}�X�Ȃ�0)
			int cost = (m_cell[next_ix] == EMPTY) ? 1 : 0;
			int new_dist = current_dist + cost;
			// ���Z���o�H�����������ꍇ�̂ݍX�V
			if (m_dist[next_ix] == UNCONNECT || new_dist < m_dist[next_ix]) {
				m_dist[next_ix] = new_dist;
				if (cost == 0) {
					q.push_front(next_ix); // �R�X�g0�͐擪��
				} else {
					q.push_back(next_ix);  // �R�X�g1�͖�����
				}
			}
		}
		if( bridge ) {
			struct SBridge {
				int		m_emp1;		//	�󗓈ʒu�ւ̃I�t�Z�b�g
				int		m_emp2;		//	�󗓈ʒu�ւ̃I�t�Z�b�g
				int		m_next;		//	�u���b�W��ւ̃I�t�Z�b�g
			};
/*
                  -2W+1
        -W-1 -W   -W+1 -W+2
        -1   0    +1
   +W-2 +W-1 +W   +W+1
        2W-1
*/
			const int W = m_ary_width;
			const SBridge offsets[] = {
				{-W, -W+1, -2*W+1},
				{-W+1, 1, -W+2},
				{W, 1, W+1},
				{W, W-1, 2*W-1},
				{-1, W-1, W-2},
				{-1, -W, -W-1},
			};
			for(const auto& ofst: offsets) {
				int emp1_ix = cur_ix + ofst.m_emp1;
				int emp2_ix = cur_ix + ofst.m_emp2;
				int next_ix = cur_ix + ofst.m_next;
				if( m_cell[emp1_ix] != EMPTY || m_cell[emp2_ix] != EMPTY || 
					m_cell[next_ix] == WALL || m_cell[next_ix] == opp_color )
				{
					continue;
				}
				int cost = (m_cell[next_ix] == EMPTY) ? 1 : 0;
				int new_dist = current_dist + cost;
				// ���Z���o�H�����������ꍇ�̂ݍX�V
				if (m_dist[next_ix] == UNCONNECT || new_dist < m_dist[next_ix]) {
					m_dist[next_ix] = new_dist;
					if (cost == 0) {
						q.push_front(next_ix); // �R�X�g0�͐擪��
					} else {
						q.push_back(next_ix);  // �R�X�g1�͖�����
					}
				}
			}
		}
	}
	//print_dist();
	return min_dist;
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
#if 0
		if( union_find(ix, next) ) {
			m_last_put_ix = ix;
			break;
		}
#else
		if( next == BLACK && is_vert_connected() ||
			next == WHITE && is_horz_connected() )
		{
			m_last_put_ix = ix;
			break;
		}
#endif
		next = (BLACK+WHITE) - next;
	}
	return next == n0;
};
bool Board::union_find(int ix, Color col) {	//	return: ����ɂ��㉺ or ���E�ӂ��A�����ꂽ���H
#if 1
	if( col == BLACK ) {
		int x = ixToY(ix);
		if( x == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	��ӂɐڑ�
		else if( x == m_bd_width - 1 )
			m_parent_ul[ix] = BT_INDEX;		//	���ӂɐڑ�
	} else {	//	col == WHITE
		int y = ixToX(ix);
		if( y == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	���ӂɐڑ�
		else if( y == m_bd_width - 1 )
			m_parent_ul[ix] = RT_INDEX;		//	�E�ӂɐڑ�
	}
	check_connected_uf(ix, ix-m_ary_width, col);
	check_connected_uf(ix, ix- m_ary_width +1, col);
	check_connected_uf(ix, ix-1, col);
	check_connected_uf(ix, ix+1, col);
	check_connected_uf(ix, ix+ m_ary_width -1, col);
	check_connected_uf(ix, ix+ m_ary_width, col);
	return m_parent_ul[BT_INDEX] == TL_INDEX || m_parent_ul[RT_INDEX] == TL_INDEX;
#if 0
	//	���Ӂi�E�Ӂj�̐΂���Ӂi���Ӂj�ɘA�����Ă��邩�H
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
			m_parent_ul[ix] = TL_INDEX;		//	��ӂɐڑ�
		else if( y == m_bd_width- 1 )
			m_parent_dr[ix] = DR_INDEX;		//	���ӂɐڑ�
	} else {	//	col == WHITE
		auto x = ixToX(ix);
		if( x == 0 )
			m_parent_ul[ix] = TL_INDEX;		//	���ӂɐڑ�
		else if( x == m_bd_width - 1 )
			m_parent_dr[ix] = DR_INDEX;		//	�E�ӂɐڑ�
	}
	check_connected_uf(ix, ix-m_ary_width, col);
	check_connected_uf(ix, ix- m_ary_width +1, col);
	check_connected_uf(ix, ix-1, col);
	check_connected_uf(ix, ix+1, col);
	check_connected_uf(ix, ix+ m_ary_width -1, col);
	check_connected_uf(ix, ix+ m_ary_width, col);
	if( m_parent_ul[ix] < 0 ) {	//	6�ߖT�ɔ�ڑ�
		m_parent_ul[ix] = ix;
	}
	if( m_parent_dr[ix] < 0 ) {	//	6�ߖT�ɔ�ڑ�
		m_parent_dr[ix] = ix;
	}
	return find_root_ul(ix) == TL_INDEX && find_root_dr(ix) == DR_INDEX;
#endif
}
void Board::check_connected_uf(int ix, int ix2, Color col) {
#if 1
	if( m_cell[ix2] != col ) return;	//	��ڑ�
	if( m_parent_ul[ix] == ix ) {				//	ix ���ڑ������ς݂ł͂Ȃ�
		m_parent_ul[ix] = find_root_ul(ix2);
	} else {		//	ix ���ڑ������ς� �� �}�[�W����
		// ���ꂼ��̃O���[�v�̍��i��\ID�j���擾
		int root1 = find_root_ul(ix);
		int root2 = find_root_ul(ix2);
		if (root1 != root2) {
			// �����قȂ�ꍇ�̂݃}�[�W�A����������ID�����ɂ���
			if (root1 < root2) {
				m_parent_ul[root2] = root1;
			} else {
				m_parent_ul[root1] = root2;
			}
		}
	}
#else
	if( m_cell[ix2] != col ) return;	//	��ڑ�
	if( m_parent_ul[ix] == UNCONNECT ) {				//	ix ���ڑ������ς݂ł͂Ȃ�
		m_parent_ul[ix] = find_root_ul(ix2);
	} else {		//	ix ���ڑ������ς� �� �}�[�W����
		// ���ꂼ��̃O���[�v�̍��i��\ID�j���擾
		int root1 = find_root_ul(ix);
		int root2 = find_root_ul(ix2);
		if (root1 != root2) {
			// �����قȂ�ꍇ�̂݃}�[�W�A����������ID�����ɂ���
			if (root1 < root2) {
				m_parent_ul[root2] = root1;
			} else {
				m_parent_ul[root1] = root2;
			}
		}
	}
	if( m_parent_dr[ix] == UNCONNECT ) {				//	ix ���ڑ������ς݂ł͂Ȃ�
		m_parent_dr[ix] = find_root_dr(ix2);
	} else {		//	ix ���ڑ������ς� �� �}�[�W����
		// ���ꂼ��̃O���[�v�̍��i��\ID�j���擾
		int root1 = find_root_dr(ix);
		int root2 = find_root_dr(ix2);
		if (root1 != root2) {
			// �����قȂ�ꍇ�̂݃}�[�W�A�傫������ID�����ɂ���
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
	//	�ċA�I�ɍ���T���A�r���̃m�[�h�����ɒ��ڂȂ��ւ���i�o�H���k�j
	return m_parent_ul[ix] = find_root_ul(m_parent_ul[ix]);
}
//int Board::find_root_dr(int ix) {
//	if(ix < 0 || m_parent_dr[ix] == ix ) return ix;
//	//	�ċA�I�ɍ���T���A�r���̃m�[�h�����ɒ��ڂȂ��ւ���i�o�H���k�j
//	return m_parent_dr[ix] = find_root_dr(m_parent_dr[ix]);
//}

int Board::sel_move_random() const {
	vector<int> lst;
	get_empty_indexes(lst);
	if( lst.is_empty() ) return -1;
	return lst[rgen() % lst.size()];
}
int Board::sel_move_PMC(Color next, int limit) const {	//	limit: �v�l���� �P�ʁF�~���b
	vector<int> lst;
	get_empty_indexes(lst);
	if( lst.is_empty() ) return -1;
	Board b2(this->m_bd_width);
	// --- ���Ԍv���̏��� ---
	m_startTime = std::chrono::high_resolution_clock::now();
	m_timeLimit = limit;
	m_timeOver = false;
	m_nodesSearched = 0;
	//
	int np = 0;
	vector<int> nwon(lst.size(), 0);
	for(;;) {
		for(int i = 0; i != lst.size(); ++i) {
			b2 = *this;
			b2.set_color(lst[i], next);
			bool b = b2.playout_to_full((BLACK+WHITE)-next);
			if( !b )
				nwon[i] += 1;
		}
		++np;
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
        if (duration >= m_timeLimit)
        	break;
	}
	int best_ix = -1;
	int max_nw = -1;
	for(int i = 0; i != lst.size(); ++i) {
		if( nwon[i] > max_nw ) {
			max_nw = nwon[i];
			best_ix = lst[i];
		}
	}
	cout << "n_won / n_playout = " << max_nw << "/" << np << " = " << (double)max_nw/np <<  endl << endl;
	return best_ix;
}
void Board::do_DFS(Color next, int depth) {		//	depth == 0 �ɂȂ�܂Ő[���D��T��
	m_nodesSearched = 0;
	DFS_recursive(next, depth);
}
void Board::DFS_recursive(Color next, int depth) {		//	depth == 0 �ɂȂ�܂Ő[���D��T��
	if( depth == 0 ) {
		m_nodesSearched += 1;
		return;
	}
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			DFS_recursive((BLACK+WHITE)-next, depth-1);
			m_cell[ix] = EMPTY;
		}
	}
}
int Board::do_itrdeep(Color next, int limit) {		//	
	build_zobrist_table();
	m_hash_val = 0;			//	���ǖʂ̃n�b�V���l���O��
	m_tt.clear();			//	�u���\�N���A
	// --- ���Ԍv���̏��� ---
	m_startTime = std::chrono::high_resolution_clock::now();
	m_timeLimit = limit;
	m_timeOver = false;
	m_nodesSearched = 0;
	for(int depth = 1; ; ++depth) {
		itrdeep_recursive(next, depth);
		if( m_timeOver ) {
			return depth;
		}
	}
}
void Board::itrdeep_recursive(Color next, int depth) {		//	depth == 0 �ɂȂ�܂Ő[���D��T��
	if ((++m_nodesSearched & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
        if (duration >= m_timeLimit) {
            m_timeOver = true;
            return;
        }
    }
	if( depth == 0 ) {
		return;
	}
	TTEntry& entry = m_tt[m_hash_val];		//	���ǖʂ����o�^�̏ꍇ�́A�v�f�������I�ɒǉ������
	if( entry.m_depth >= depth )			//	���ǖʂ͒T���ς�
		return;
	for(int ix = xyToIndex(0, 0); ix <= xyToIndex(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
			itrdeep_recursive((BLACK+WHITE)-next, depth-1);
			m_cell[ix] = EMPTY;
			m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
			if( m_timeOver ) break;
		}
	}
	entry.m_depth = depth;
}
void Board::build_zobrist_table() {
	if( !m_zobrist_black.is_empty() ) return;	//	�������ς�
	m_zobrist_black.resize(m_ary_size);
	for(auto& r: m_zobrist_black) r = rgen64();
	m_zobrist_white.resize(m_ary_size);
	for(auto& r: m_zobrist_white) r = rgen64();
}
