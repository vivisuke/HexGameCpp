#include <iostream>
#include <string>
#include <algorithm>
#include <random>
#include <queue>
#include <assert.h>
#include "Board.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 
//static std::mt19937 rgen(0); 
std::mt19937_64 rgen64(rd());		// 64�r�b�g��

#define		is_empty()	empty()

Color opp_color(Color col) { return (BLACK+WHITE) - col; }

struct SBridge {
	int		m_emp1;		//	�󗓈ʒu�ւ̃I�t�Z�b�g
	int		m_emp2;		//	�󗓈ʒu�ւ̃I�t�Z�b�g
	int		m_next;		//	�u���b�W��ւ̃I�t�Z�b�g
};

Board::Board(int width)
    : m_bd_width(width), m_ary_width(width + 1)
    , m_ary_height(width + 2), m_ary_size((width + 1)* (width + 2))
{
    m_cell.resize(m_ary_size);  // �ԕ����P�����z�񃁃����m��
	m_parent_ul.resize(m_ary_size);
	//m_parent_dr.resize(m_ary_size);
    init();  // �Ֆʏ�����
	build_fixed_order();
	build_zobrist_table();
	//	��]�p�e�[�u��������
	m_rot180_table.resize(m_ary_size);
	for(int y = 0; y < m_bd_width; ++y) {
		for(int x = 0; x < m_bd_width; ++x) {
			int ix = xyToIX(x, y);
			int ix2 = xyToIX(m_bd_width-1-x, m_bd_width-1-y);
			m_rot180_table[ix] = ix2;
		}
	}
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
	m_last_put_ix = 0;
}
Board& Board::operator=(const Board& s) {
	m_cell = s.m_cell;

	return *this;
}
string Board::ixToStr(int ix) const {
	if( ix < 0 ) return "-1";
	string txt = "a1";
	txt[0] = 'a' + ixToX(ix);
	txt[1] = '1' + ixToY(ix);
	return txt;
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
void Board::print_dist() const {
	for(int y = 0; y < m_bd_width; ++y) {
		cout << string(y*2, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < m_bd_width; ++x) {
			auto d = m_dist[xyToIX(x, y)];
			if( d == UNCONNECT) cout << "  -1";
			else printf("%4d", d);
		}
		cout << endl;
	}
	cout << endl;
}
void Board::get_tt_best_moves(Color next, vector<int>& moves) {
	moves.clear();
	get_tt_best_moves_sub(next, moves);
}
void Board::get_tt_best_moves_sub(Color next, vector<int>& moves) {
	const TTEntry& entry = m_tt[m_hash_val];
	if( entry.m_flag == FLAG_UNKNOWN || entry.m_best_move == 0 )
		return;
	moves.push_back(entry.m_best_move);
	auto ix = entry.m_best_move;
	set_color(ix, next);
	m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	get_tt_best_moves_sub((BLACK+WHITE)-next, moves);
	set_color(ix, EMPTY);
	m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
}
void Board::print_tt(Color next) {
	const TTEntry& entry = m_tt[m_hash_val];
	//cout << "eval = " << entry.m_score << " ";
	printf("eval = %6.2f ", entry.m_score);
	print_tt_sub(next);
	cout << endl;
}
void Board::print_tt_sub(Color next) {
	const TTEntry& entry = m_tt[m_hash_val];
	if( entry.m_flag == FLAG_UNKNOWN || entry.m_best_move == 0 )
		return;
	cout << ixToStr(entry.m_best_move) << " ";
	auto ix = entry.m_best_move;
	set_color(ix, next);
	m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	print_tt_sub((BLACK+WHITE)-next);
	set_color(ix, EMPTY);
	m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
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
void Board::swap_black_white() {
	for(int y = 0; y < m_bd_width-1; ++y) {
		for(int x = 0; x < m_bd_width-1-y; ++x) {
			int ix1 = xyToIX(x, y);	//	�Ίp���̍��㑤
			int ix2 = xyToIX(m_bd_width-1-y, m_bd_width-1-x);	//	�Ίp���̍��㑤
			auto t1 = m_cell[ix1];
			auto t2 = m_cell[ix2];
			if( t1 != EMPTY ) t1 = (BLACK+WHITE) - t1;
			if( t2 != EMPTY ) t2 = (BLACK+WHITE) - t2;
			m_cell[ix1] = t2;
			m_cell[ix2] = t1;
		}
	}
	for(int y = 0; y < m_bd_width; ++y) {
		int ix = xyToIX(m_bd_width-1-y, y);
		if( m_cell[ix] != EMPTY ) 
			m_cell[ix] = (BLACK+WHITE) - m_cell[ix];
	}
}
int Board::swap_bw_ix(int ix) const {
	int x = ixToX(ix);
	int y = ixToY(ix);
	return xyToIX(m_bd_width-1-y, m_bd_width-1-x);
}
bool Board::is_vert_connected_BFS() const {
	// �T���p�̃L���[
    std::queue<int> q;
    // �T���ς݃t���O���Ǘ�����z��
    std::vector<char> visited(m_cell.size(), false);
    // 1. �n�_�̐ݒ�F��ӂɂ���S�Ă̍��΂��L���[�ɒǉ�
    for (int x = m_bd_width; --x >= 0;) {
        int ix = xyToIX(x, 0);
        if (m_cell[ix] == BLACK) {
            q.push(ix);
            visited[ix] = true;
        }
    }
	const int offsets[] = {
        -m_ary_width,      // ��
        -m_ary_width + 1,  // �E��
        -1,                 // ��
        +1,                 // �E
        +m_ary_width - 1,  // ����
        +m_ary_width       // ��
    };
	// 2. BFS�̎��s
    while (!q.empty()) {
        int current_ix = q.front();
        q.pop();
        // (x, y) ���W�ɕϊ��i���Ӄ`�F�b�N�̂��߁j
        int y = ixToY(current_ix);
        // 3. �I�������F���ӂɓ��B�������H
        if (y == m_bd_width - 1) {
            return true; // �ڑ������I
        }
        // �אڂ���6�����𒲂ׂ�
        for (int offset : offsets) {
            int next_ix = current_ix + offset;
            
            // �אڃ}�X���Ֆʓ��ŁA���΂ł���A�����K�₩�H
            // (�ԕ��̂������ŔՖʓ��O�̃`�F�b�N�͕s�v)
            if (m_cell[next_ix] == BLACK && !visited[next_ix]) {
                visited[next_ix] = true;
                q.push(next_ix);
            }
        }
    }
	return false;
}
bool Board::is_horz_connected_BFS() const {
	// �T���p�̃L���[
    std::queue<int> q;
    // �T���ς݃t���O���Ǘ�����z��
    std::vector<char> visited(m_cell.size(), false);
    // 1. �n�_�̐ݒ�F���ӂɂ���S�Ă̔��΂��L���[�ɒǉ�
    for (int y = m_bd_width; --y >= 0;) {
        int ix = xyToIX(0, y);
        if (m_cell[ix] == WHITE) {
            q.push(ix);
            visited[ix] = true;
        }
    }
	const int offsets[] = {
        -m_ary_width,      // ��
        -m_ary_width + 1,  // �E��
        -1,                 // ��
        +1,                 // �E
        +m_ary_width - 1,  // ����
        +m_ary_width       // ��
    };
	// 2. BFS�̎��s
    while (!q.empty()) {
        int current_ix = q.front();
        q.pop();
        // (x, y) ���W�ɕϊ��i���Ӄ`�F�b�N�̂��߁j
        int x = ixToX(current_ix);
        // 3. �I�������F�E�ӂɓ��B�������H
        if (x == m_bd_width - 1) {
            return true; // �ڑ������I
        }
        // �אڂ���6�����𒲂ׂ�
        for (int offset : offsets) {
            int next_ix = current_ix + offset;
            
            // �אڃ}�X���Ֆʓ��ŁA���΂ł���A�����K�₩�H
            // (�ԕ��̂������ŔՖʓ��O�̃`�F�b�N�͕s�v)
            if (m_cell[next_ix] == WHITE && !visited[next_ix]) {
                visited[next_ix] = true;
                q.push(next_ix);
            }
        }
    }
	return false;
}
bool Board::is_vert_connected_v_BFS() const {		//	���z�A���l����
	// �T���p�̃L���[
    std::queue<int> q;
    // �T���ς݃t���O���Ǘ�����z��
    std::vector<char> visited(m_cell.size(), false);
    // 1. �n�_�̐ݒ�F��ӂɂ���S�Ă̍��΂��L���[�ɒǉ�
    for (int x = m_bd_width; --x >= 0;) {
        int ix = xyToIX(x, 0);
        if (m_cell[ix] == BLACK) {
            q.push(ix);
            visited[ix] = true;
        }
    }
    //	��ӂ̂Q�̋󗓁{���΂��L���[�ɒǉ�
    for (int x = m_bd_width; --x >= 1;) {
        int ix = xyToIX(x, 0);
        int ix2 = ix+m_ary_width-1;
        if (m_cell[ix] == EMPTY && m_cell[ix-1] == EMPTY && m_cell[ix2] == BLACK) {
            q.push(ix2);
            visited[ix2] = true;
        }
    }
	const int offsets[] = {
        -m_ary_width,      // ��
        -m_ary_width + 1,  // �E��
        -1,                 // ��
        +1,                 // �E
        +m_ary_width - 1,  // ����
        +m_ary_width       // ��
    };
/*
                  -2W+1
        -W-1 -W   -W+1 -W+2
        -1   0    +1
   +W-2 +W-1 +W   +W+1
        2W-1
*/
	const int W = m_ary_width;
	const int offsets_v[][3] = {
		{-W, -W+1, -2*W+1},
		{-W+1, 1, -W+2},
		{W, 1, W+1},
		{W, W-1, 2*W-1},
		{-1, W-1, W-2},
		{-1, -W, -W-1},
	};
	// 2. BFS�̎��s
    while (!q.empty()) {
        int current_ix = q.front();
        q.pop();
        // (x, y) ���W�ɕϊ��i���Ӄ`�F�b�N�̂��߁j
        int y = ixToY(current_ix);
        // 3. �I�������F���ӂɓ��B�������H
        if (y == m_bd_width - 1) {
            return true; // �ڑ������I
        }
        //	�Q�̋󗓂������ĉ��ӂɉ��z�A�����Ă��邩�H
        else if( y == m_bd_width - 2 && m_cell[current_ix+m_ary_width-1] == EMPTY &&
        			m_cell[current_ix+m_ary_width] == EMPTY)
        {
            return true; // �ڑ������I
        }
        // �אڂ���6�����𒲂ׂ�
        for (int offset : offsets) {
            int next_ix = current_ix + offset;
            // �אڃ}�X���Ֆʓ��ŁA���΂ł���A�����K�₩�H
            if (m_cell[next_ix] == BLACK && !visited[next_ix]) {
                visited[next_ix] = true;
                q.push(next_ix);
            }
        }
        // ���z�A������6�����𒲂ׂ�
        for (const auto ofst : offsets_v) {
            if( m_cell[current_ix + ofst[0]] == EMPTY && m_cell[current_ix + ofst[1]] == EMPTY ) {
	            int next_ix = current_ix + ofst[2];
	            // �אڃ}�X���Ֆʓ��ŁA���΂ł���A�����K�₩�H
	            if (m_cell[next_ix] == BLACK && !visited[next_ix]) {
	                visited[next_ix] = true;
	                q.push(next_ix);
	            }
            }
        }
    }
	return false;
}
bool Board::is_horz_connected_v_BFS() const {		//	���z�A���l����
	// �T���p�̃L���[
    std::queue<int> q;
    // �T���ς݃t���O���Ǘ�����z��
    std::vector<char> visited(m_cell.size(), false);
    // 1. �n�_�̐ݒ�F���ӂɂ���S�Ă̔��΂��L���[�ɒǉ�
    for (int y = m_bd_width; --y >= 0;) {
        int ix = xyToIX(0, y);
        if (m_cell[ix] == WHITE) {
            q.push(ix);
            visited[ix] = true;
        }
    }
    //	���ӂ̂Q�̋󗓁{���΂��L���[�ɒǉ�
    for (int y = m_bd_width; --y >= 1;) {
        int ix = xyToIX(0, y);
        int ix2 = ix-m_ary_width+1;
        if (m_cell[ix] == EMPTY && m_cell[ix-m_ary_width] == EMPTY && m_cell[ix2] == WHITE) {
            q.push(ix2);
            visited[ix2] = true;
        }
    }
	const int offsets[] = {
        -m_ary_width,      // ��
        -m_ary_width + 1,  // �E��
        -1,                 // ��
        +1,                 // �E
        +m_ary_width - 1,  // ����
        +m_ary_width       // ��
    };
/*
                  -2W+1
        -W-1 -W   -W+1 -W+2
        -1   0    +1
   +W-2 +W-1 +W   +W+1
        2W-1
*/
	const int W = m_ary_width;
	const int offsets_v[][3] = {
		{-W, -W+1, -2*W+1},
		{-W+1, 1, -W+2},
		{W, 1, W+1},
		{W, W-1, 2*W-1},
		{-1, W-1, W-2},
		{-1, -W, -W-1},
	};
	// 2. BFS�̎��s
    while (!q.empty()) {
        int current_ix = q.front();
        q.pop();
        // (x, y) ���W�ɕϊ��i���Ӄ`�F�b�N�̂��߁j
        int x = ixToX(current_ix);
        // 3. �I�������F���ӂɓ��B�������H
        if (x == m_bd_width - 1) {
            return true; // �ڑ������I
        }
        //	�Q�̋󗓂������ĉ��ӂɉ��z�A�����Ă��邩�H
        else if( x == m_bd_width - 2 && m_cell[current_ix+1] == EMPTY &&
        			m_cell[current_ix-m_ary_width+1] == EMPTY)
        {
            return true; // �ڑ������I
        }
        // �אڂ���6�����𒲂ׂ�
        for (int offset : offsets) {
            int next_ix = current_ix + offset;
            // �אڃ}�X���Ֆʓ��ŁA���΂ł���A�����K�₩�H
            if (m_cell[next_ix] == WHITE && !visited[next_ix]) {
                visited[next_ix] = true;
                q.push(next_ix);
            }
        }
        // ���z�A������6�����𒲂ׂ�
        for (const auto ofst : offsets_v) {
            if( m_cell[current_ix + ofst[0]] == EMPTY && m_cell[current_ix + ofst[1]] == EMPTY ) {
	            int next_ix = current_ix + ofst[2];
	            // �אڃ}�X���Ֆʓ��ŁA���΂ł���A�����K�₩�H
	            if (m_cell[next_ix] == WHITE && !visited[next_ix]) {
	                visited[next_ix] = true;
	                q.push(next_ix);
	            }
            }
        }
    }
	return false;
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
	for(int x = 0; x < m_bd_width; ++x) {
		if( is_vert_connected_v_DFS(xyToIX(x, 0)) )		//	�[���D��T��
			return true;	//	�㉺�A���o�H�𔭌������ꍇ
	}
	return false;
}
bool Board::is_vert_connected_v_DFS(int ix) const {
	if( m_cell[ix] != BLACK || m_connected[ix] != UNSEARCHED )	//	���łȂ� or �T���ς�
		return false;
	if( ix >= xyToIX(0, m_bd_width-1) )		//	���ӂɓ��B�����ꍇ
		return true;
	m_connected[ix] = SEARCHED;
	return	is_vert_connected_v_DFS(ix + m_ary_width) ||
			is_vert_connected_v_DFS(ix + m_ary_width - 1) ||
			is_vert_connected_v_DFS(ix + 1) ||
			is_vert_connected_v_DFS(ix - 1) ||
			is_vert_connected_v_DFS(ix - m_ary_width + 1) ||
			is_vert_connected_v_DFS(ix - m_ary_width);
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
	// 1. �X�^�[�g�n�_�i��E���Ӂj���L���[�ɒǉ�
	for (int i = 0; i < m_bd_width; ++i) {
		int ix = vertical ? xyToIX(i, 0) : xyToIX(0, i);
		if (m_cell[ix] == own_color) {
			m_dist[ix] = 0;
			q.push_front(ix); // �R�X�g0�Ȃ̂Ő擪�ɒǉ�
		} else if(m_cell[ix] == EMPTY) {
			m_dist[ix] = 1;
			q.push_back(ix);  // �R�X�g1�Ȃ̂Ŗ����ɒǉ�
		}
	}
	if( bridge ) {
		bool is_emp0 = m_cell[xyToIX(0, 0)] == EMPTY;	//	�󗓂��H
		for (int i = 1; i < m_bd_width; ++i) {
			int ix = vertical ? xyToIX(i, 0) : xyToIX(0, i);
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
//	���̎�ԁinext�j���猩���]���l�i�L���ł���΃v���X�j��Ԃ�
float Board::eval(Color next) {
	auto vd = calc_vert_dist(false);		//	�U�ߖT ���ژA������
	auto hd = calc_horz_dist(false);		//	�U�ߖT ���ژA������
	auto bvd = calc_vert_dist(true);		//	�U�ߖT�{�u���b�W �A������
	auto bhd = calc_horz_dist(true);		//	�U�ߖT�{�u���b�W �A������
	if( next == WHITE ) {
		swap(vd, hd);
		swap(bvd, bhd);
	}
	auto n_emp = n_empty();		//	�󗓐�
	if( vd == 0 ) {				//	next ���F�ӂ�A���ς�
		return 1 + n_emp;
	}
	if( hd == 0 ) {				//	���葤�F�ӂ�A���ς�
		return -1 - n_emp;
	}
	if( vd == 1 ) {				//	next ���F���̂P��ŕӂ�A���\
		return n_emp;
	}
	if( bvd == 0 ) {			//	next ��: �����m��
		return 2 + n_emp - vd*2;
	}
	if( bhd == 0 ) {			//	���葤: �����m��
		return -(1 + n_emp - hd*2);
	}
	if( bvd == 1 ) {			//	next ��: ���̂P��ŏ����m��ɂł���
		return n_emp - (vd-1)*2;
	}
	return bhd - bvd + (hd - vd)/100.0 + 0.5;
}
float Board::nega_max(Color next, int depth) {
	if( next == WHITE && calc_vert_dist(false) == 0 ||
		next == BLACK && calc_horz_dist(false) == 0 )
	{
		return -(n_empty() + 1);		//	��ԂłȂ������������Ă�
	}
	if( depth <= 0 ) {
		return eval(next);
	}
	vector<int> lst;		//	�󗓈ʒu���X�g
	get_empty_indexes(lst);
	if( lst.is_empty() ) {	//	�󗓖����̏ꍇ
		return eval(next);
	}
	float maxev = std::numeric_limits<float>::lowest();
	const Color nn = (BLACK+WHITE) - next;
	for(auto ix: lst) {
		set_color(ix, next);
		maxev = max(maxev, -nega_max(nn, depth-1));
		set_color(ix, EMPTY);
	}
	return maxev;
}
void Board::get_empty_indexes(vector<int>& lst) const {
	lst.clear();
	for(int ix = xyToIX(0, 0); ix <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY )
			lst.push_back(ix);
	}
}
void Board::get_subdiagonal_indexes(vector<int>& lst) const {
	lst.clear();
	for(int y = 0; y < m_bd_width; ++y) {
		int ix = xyToIX(m_bd_width-y-1, y);
		if (m_cell[ix] == EMPTY)
			lst.push_back(ix);
	}
	shuffle(lst.begin(), lst.end(), rgen);
}
void Board::add_bridge_indexes(vector<int>& lst, int ix0) const {
	//	�u���b�W��T��
	const int W = m_ary_width;
	const SBridge boffsets[] = {
		{-W, -W+1, -2*W+1},
		{-W+1, 1, -W+2},
		{W, 1, W+1},
		{W, W-1, 2*W-1},
		{-1, W-1, W-2},
		{-1, -W, -W-1},
	};
	for(const auto& ofst: boffsets) {
		int emp1_ix = ix0 + ofst.m_emp1;
		int emp2_ix = ix0 + ofst.m_emp2;
		int next_ix = ix0 + ofst.m_next;
		if( m_cell[emp1_ix] == EMPTY && m_cell[emp2_ix] == EMPTY &&  m_cell[next_ix] == EMPTY )
			lst.push_back(next_ix);
	}
}
void Board::get_local_indexes(vector<int>& lst, int last_ix, int last2_ix) const {
	lst.clear();
	if( last_ix == 0 ) {	//	���O�肪�����ꍇ�́A���Ίp����Z��������
		for(int y = 0; y < m_bd_width; ++y) {
			int ix = xyToIX(m_bd_width-y-1, y);
			if (m_cell[ix] == EMPTY)
				lst.push_back(ix);
		}
	} else {
		add_bridge_indexes(lst, last_ix);
		if( last2_ix != 0 )
			add_bridge_indexes(lst, last2_ix);
		const int offsets[] = {	// �אڃZ���̃C���f�b�N�X����
			-1, +1, -m_ary_width, +m_ary_width, -m_ary_width + 1, +m_ary_width - 1
		};
		for (int offset : offsets) {	// �אڃZ����T��
			int ix = last_ix + offset;
			if (m_cell[ix] == EMPTY)
				lst.push_back(ix);
		}
		if( last2_ix != 0 ) {
			for (int offset : offsets) {	// �אڃZ����T��
				int ix = last2_ix + offset;
				if (m_cell[ix] == EMPTY)
					lst.push_back(ix);
			}
		}
	}
	if( lst.is_empty() )
		get_empty_indexes(lst);
}
void Board::local_playout(Color next, int ix) {
	vector<int> lst;
	int last2_ix = 0;
	for(;;) {
		get_local_indexes(lst, ix, last2_ix);
		if( lst.is_empty() )
			break;
		last2_ix = ix;
		ix = lst[rgen() % lst.size()];
		set_color(ix, next);
		next = (BLACK+WHITE) - next;
	}
}
bool Board::local_playout_to_full(Color next) {
	local_playout(next);
	if( next == BLACK )
		return is_vert_connected();
	else
		return !is_vert_connected();
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
			int ix = xyToIX(x, m_bd_width-1);
			if( m_cell[ix] == BLACK && find_root_ul(ix) == TL_INDEX )
				return true;
		}
	} else {
		for(int y = 0; y < m_bd_width; ++y) {
			int ix = xyToIX(m_bd_width-1, y);
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
int Board::sel_move_local_MC(Color next, int last_ix, int last2_ix, int limit) const {
	vector<int> lst;
	get_local_indexes(lst, last_ix, last2_ix);
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
	for(int ix = xyToIX(0, 0); ix <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix) {
		if( m_cell[ix] == EMPTY ) {
			m_cell[ix] = next;
			DFS_recursive((BLACK+WHITE)-next, depth-1);
			m_cell[ix] = EMPTY;
		}
	}
}
int Board::sel_move_itrdeep(Color next, int limit) const {		//	limit: �~���b�P��
	Board b2(m_bd_width);
	b2 = *this;
	b2.build_zobrist_table();
	// --- ���Ԍv���̏��� ---
	b2.m_startTime = std::chrono::high_resolution_clock::now();
	b2.m_timeLimit = limit;
	b2.m_timeOver = false;
	b2.m_nodesSearched = 0;

	b2.m_hash_val = 0;			//	���ǖʂ̃n�b�V���l���O��
	b2.m_tt.clear();			//	�u���\�N���A
	TTEntry& root = b2.m_tt[m_hash_val];
	auto alpha = std::numeric_limits<float>::lowest();
	auto beta = std::numeric_limits<float>::max();
	vector<int> moves, moves0;
	bool first_move = b2.n_empty() == m_bd_width*m_bd_width;
	for(int depth = 1; depth <= 1000; ++depth) {
		//nega_max_tt(next, depth);
		b2.nega_alpha_tt(next, depth, alpha, beta, first_move);
		b2.get_tt_best_moves(next, moves);
		if( moves == moves0 ) break;
		moves0.swap(moves);
		b2.print_tt(next);
		if( b2.m_timeOver ) break;
	}
	cout << "nodesSearched = " << m_nodesSearched << endl;
	return root.m_best_move;
}
//	�u���\�𗘗p���� nega_alpha()
float Board::nega_alpha_tt(Color next, int depth, float alpha, float beta, bool first_move) {
	if ((++m_nodesSearched & 2047) == 0) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
        if (duration >= m_timeLimit) {
            m_timeOver = true;
            return 0;
        }
    }
	TTEntry& entry = m_tt[m_hash_val];		//	���ǖʂ����o�^�̏ꍇ�́A�v�f�������I�ɒǉ������
	if( entry.m_flag == FLAG_TERMINAL )		//	�m��]���l�̏ꍇ
		return entry.m_score;
	if( next == WHITE && calc_vert_dist(false) == 0 ||
		next == BLACK && calc_horz_dist(false) == 0 )
	{
		entry.m_flag = FLAG_TERMINAL;				//	�]���l�m��
		return entry.m_score = -(n_empty() + 1);	//	��ԂłȂ������������Ă�
	}
	if( depth <= 0 ) {
		entry.m_flag = FLAG_EXACT;				//	�]���l
		return entry.m_score = eval(next);
	}
	if( entry.m_depth >= depth ) {	//	���ł� depth �ŒT���ς�
		//return entry.m_score;
		if ( entry.m_flag == FLAG_EXACT )
			return entry.m_score;
		if ( entry.m_flag == FLAG_LOWER ) {
			if ( entry.m_score >= beta ) return entry.m_score; // ���̎}�������
			alpha = std::max(alpha, entry.m_score);
		} else if ( entry.m_flag == FLAG_UPPER ) {
			if ( entry.m_score <= alpha ) return entry.m_score; // ���̎}�������
			beta = std::min(beta, entry.m_score);
		}
	}
	vector<int> lst;		//	�󗓈ʒu���X�g
	if( first_move )
		get_subdiagonal_indexes(lst);
	else
		get_empty_indexes(lst);
	if( lst.is_empty() ) {	//	�󗓖����̏ꍇ
		entry.m_flag = FLAG_TERMINAL;				//	�]���l�m��
		return entry.m_score = eval(next);
	}
	if( entry.m_best_move != 0 ) {
		//	undone: �őP�肪�ۑ�����Ă���΁A������ŏ��ɕ]��
		auto itr = std::find(lst.begin(), lst.end(), entry.m_best_move);
		//assert( itr != lst.end() );		//	m_best_move �͕K���܂܂�Ă���͂�
		if( itr != lst.begin() && itr != lst.end() )
			swap(*itr, lst[0]);
	}
	auto alpha_org = alpha;
	//float maxev = std::numeric_limits<float>::lowest();
	int bestix = 0;
	const byte nn = (BLACK + WHITE) - next;
	for(auto ix: lst) {
		set_color(ix, next);
		m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
		//alpha = max(alpha, -nega_alpha_tt(nn, depth-1, -beta, -alpha));
		auto ev = -nega_alpha_tt(nn, depth-1, -beta, -alpha);
		if( m_timeOver ) return 0;
		if( ev > alpha ) {
			alpha = ev;
			bestix = ix;
		}
		set_color(ix, EMPTY);
		m_hash_val ^= next == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
		if( alpha >= beta ) {
			entry.m_flag = FLAG_LOWER;			//	�����l
			entry.m_depth = depth;
			entry.m_best_move = bestix;
			return entry.m_score = alpha;
		}
	}
	//entry.m_flag = FLAG_EXACT;				//	�]���l
	entry.m_depth = depth;
	entry.m_best_move = bestix;
	entry.m_score = alpha;

	if ( alpha <= alpha_org ) {
		// ��x��alpha���X�V����Ȃ������ꍇ�A�X�R�A�͏���l
		entry.m_flag = FLAG_UPPER;
	} else {
		// alpha���X�V���ꂽ�����J�b�g�͋N���Ȃ������ꍇ�A�X�R�A�͐��m�Ȓl
		entry.m_flag = FLAG_EXACT;
	}
	
	return alpha;
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
	for(int ix = xyToIX(0, 0); ix <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix) {
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
void Board::build_zobrist_table() const {
	if( !m_zobrist_black.is_empty() ) return;	//	�������ς�
	m_zobrist_black.resize(m_ary_size);
	for(auto& r: m_zobrist_black) r = rgen64();
	m_zobrist_white.resize(m_ary_size);
	for(auto& r: m_zobrist_white) r = rgen64();
}
void Board::build_fixed_order() {
	m_fixed_order.clear();
	for(int x = 0; x < m_bd_width; ++x) {
		int y = m_bd_width-1;
		//cout << "(" << x << ", " << y << ")" << endl;
		build_fixed_order_sub(xyToIX(x, y), m_bd_width-x);
		if( x != 0 ) {
			int y = m_bd_width - x - 1;
			//cout << "(0, " << y << ")" << endl;
			build_fixed_order_sub(xyToIX(0, y), m_bd_width-x);
		}
	}
}
void Board::build_fixed_order_sub(int ix, int len) {
	if( len%2 == 0 ) {	//	len �������̏ꍇ
		int cix = ix - (m_ary_width-1) * (len/2-1);
		for(int i = 0; i < len/2; ++i) {
			m_fixed_order.push_back(cix);
			m_fixed_order.push_back(cix - (m_ary_width-1)*(i*2 + 1));
			//cout << cix << " " << cix - (m_ary_width-1)*(i*2 + 1) << " ";
			cix += m_ary_width-1;
		}
		//cout << endl;
	} else {	//	len ����̏ꍇ
		int cix = ix - (m_ary_width-1) * (len/2);
		m_fixed_order.push_back(cix);
		//cout << cix << " ";
		for(int i = 0; i < len/2; ++i) {
			cix += m_ary_width-1;
			m_fixed_order.push_back(cix);
			m_fixed_order.push_back(cix - (m_ary_width-1)*(i*2 + 2));
			//cout << cix << " " << cix - (m_ary_width-1)*(i*2 + 2) << " ";
		}
		//cout << endl;
	}
#if 0
	while (m_cell[ix] == EMPTY) {
		cout << ix << " ";
		ix = ix - m_ary_width + 1;
	}
	cout << endl;
	while( m_cell[ix] == EMPTY ) {
		m_fixed_order.push_back(ix);
		ix = ix - m_ary_width + 1;
	}
#endif
}
bool Board::is_winning_move(int ix, Color col, int n_empty) {
	bool b = true;
	m_cell[ix] = col;
	--n_empty;
	if( n_empty == 0 ) {
		++m_nodesSearched;
		b = col == BLACK ? is_vert_connected() : !is_vert_connected();
	} else {
		for(int ix2 = xyToIX(0, 0); ix2 <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix2) {
			if( m_cell[ix2] == EMPTY ) {
				if( is_winning_move(ix2, opp_color(col), n_empty) ) {
					b = false;		//	
					break;
				}
			}
		}
	}
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_move_FO(int ix, Color col, int n_empty) {
	bool b = true;
	m_cell[ix] = col;
	--n_empty;
	if( n_empty == 0 ) {
		++m_nodesSearched;
		b = col == BLACK ? is_vert_connected() : !is_vert_connected();
	} else {
		for(int ix2: m_fixed_order) {
			if( m_cell[ix2] == EMPTY ) {
				if( is_winning_move_FO(ix2, opp_color(col), n_empty) ) {
					b = false;		//	
					break;
				}
			}
		}
	}
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_move_always_check(int ix, Color col) {
	bool b = true;
	m_cell[ix] = col;
	if( col == BLACK && is_vert_connected() ||
		col == WHITE && is_horz_connected() )
	{
		++m_nodesSearched;
	} else {
		for(int ix2 = xyToIX(0, 0); ix2 <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix2) {
			if( m_cell[ix2] == EMPTY ) {
				if( is_winning_move_always_check(ix2, opp_color(col)) ) {
					b = false;		//	
					break;
				}
			}
		}
	}
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_move_check_dist(int ix, Color col) {
	bool b = true;
	m_cell[ix] = col;
	int dist;
	if( col == BLACK ) {
		dist = calc_vert_dist(false);		//	�U�ߖT�{�u���b�W ����
	} else {
		dist = calc_horz_dist(false);		//	�U�ߖT�{�u���b�W ����
	}
	if( dist == 0 ) {	//	�I�ǂ̏ꍇ
		++m_nodesSearched;
	} else {			//	�I�ǂłȂ��ꍇ
		if( col == BLACK ) {
			dist = calc_horz_dist(false);		//	�U�ߖT�{�u���b�W ����
		} else {
			dist = calc_vert_dist(false);		//	�U�ߖT�{�u���b�W ����
		}
		if( dist <= 1 ) {		//	1��ŏ����ł���
			++m_nodesSearched;
			b = false;
		} else {
			for(int ix2 = xyToIX(0, 0); ix2 <= xyToIX(m_bd_width-1, m_bd_width-1); ++ix2) {
				if( m_cell[ix2] == EMPTY ) {
					if( is_winning_move_check_dist(ix2, opp_color(col)) ) {
						b = false;		//	
						break;
					}
				}
			}
		}
	}
	m_cell[ix] = EMPTY;
	return b;
}
bool Board::is_winning_move_check_dist_FO(int ix, Color col) {
	bool b = true;
	m_cell[ix] = col;
	m_hash_val ^= col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	int dist;
	if( col == BLACK ) {
		dist = calc_vert_dist(false);		//	�U�ߖT�{�u���b�W ����
	} else {
		dist = calc_horz_dist(false);		//	�U�ߖT�{�u���b�W ����
	}
	if( dist == 0 ) {	//	�I�ǂ̏ꍇ
		++m_nodesSearched;
	} else {			//	�I�ǂłȂ��ꍇ
		if( col == BLACK ) {
			dist = calc_horz_dist(false);		//	�U�ߖT�{�u���b�W ����
		} else {
			dist = calc_vert_dist(false);		//	�U�ߖT�{�u���b�W ����
		}
		if( dist <= 1 ) {		//	1��ŏ����ł���
			++m_nodesSearched;
			b = false;
		} else {
			for(int ix2 : m_fixed_order) {
				if( m_cell[ix2] == EMPTY ) {
					if( is_winning_move_check_dist_FO(ix2, opp_color(col)) ) {
						b = false;		//	
						break;
					}
				}
			}
		}
	}
	m_cell[ix] = EMPTY;
	m_hash_val ^= col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	return b;
}
//	ix �� col ��ł�͏������H
bool Board::is_winning_move_TT(int ix, Color col) {
	bool b = true;
	m_cell[ix] = col;
	m_hash_val ^= col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	int rix = m_rot180_table[ix];
	m_hash_val2 ^= col == BLACK ? m_zobrist_black[rix] : m_zobrist_white[rix];
	TT2Entry& entry = m_tt2[min(m_hash_val, m_hash_val2)];
	if( entry.m_flag == FLAG_TERMINAL ) {
		b = entry.m_winning;
	} else {
		int dist;
		if( col == BLACK ) {
			dist = calc_vert_dist(false);		//	�U�ߖT�{�u���b�W ����
		} else {
			dist = calc_horz_dist(false);		//	�U�ߖT�{�u���b�W ����
		}
		if( dist == 0 ) {	//	�I�ǂ̏ꍇ
			++m_nodesSearched;
		} else {			//	�I�ǂłȂ��ꍇ
			if( col == BLACK ) {
				dist = calc_horz_dist(false);		//	�U�ߖT�{�u���b�W ����
			} else {
				dist = calc_vert_dist(false);		//	�U�ߖT�{�u���b�W ����
			}
			if( dist <= 1 ) {		//	1��ŏ����ł���
				++m_nodesSearched;
				b = false;
			} else {
				for(int ix2 : m_fixed_order) {
					if( m_cell[ix2] == EMPTY ) {
						if( is_winning_move_TT(ix2, opp_color(col)) ) {
							b = false;		//	
							break;
						}
					}
				}
			}
		}
		entry.m_flag = FLAG_TERMINAL;
		entry.m_winning = b;
	}
	m_cell[ix] = EMPTY;
	m_hash_val ^= col == BLACK ? m_zobrist_black[ix] : m_zobrist_white[ix];
	m_hash_val2 ^= col == BLACK ? m_zobrist_black[rix] : m_zobrist_white[rix];
	return b;
}
