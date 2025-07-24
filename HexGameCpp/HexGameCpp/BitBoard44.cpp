#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "BitBoard44.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 

//const int BD_WIDTH = 4;
//const int BD_HEIGHT = 4;

void BitBoard44::print() const {
	cout << "   ";
	for(int x = 0; x < BD_WIDTH; ++x)
		printf("%c ", 'a'+x);
	cout << endl;
	ushort mask = 0x8000;
	for(int y = 0; y < BD_HEIGHT; ++y) {
		cout << string(y, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < BD_WIDTH; ++x) {
			if( (m_black & mask) != 0 )
				cout << "●";
			else if( (m_white & mask) != 0 )
				cout << "◯";
			else
				cout << "・";
			mask >>= 1;
		}
		cout << endl;
	}
	cout << endl;
}
byte BitBoard44::get_color(ushort mask) const {
	if( (m_black & mask) != 0 ) return BLACK;
	if( (m_white & mask) != 0 ) return WHITE;
	return EMPTY;
}
bool BitBoard44::did_black_win() const {
	ushort bc = m_black & 0xf000;		//	上辺黒石
	if( bc == 0 ) return false;
	for(;;) {
		ushort bc0 = bc;
		bc |= (bc0 << 4) & m_black;
		bc |= (bc0 << 3) & m_black & 0xeeee;
		bc |= (bc0 << 1) & m_black & 0xeeee;
		bc |= (bc0 >> 1) & m_black & 0x7777;
		bc |= (bc0 >> 3) & m_black & 0x7777;
		bc |= (bc0 >> 4) & m_black;
		if( bc == bc0 ) break;
	}
	return (bc & 0x000f) != 0;		//	下辺に達したか？
}
bool BitBoard44::did_white_win() const {
	ushort bc = m_white & 0x8888;		//	左辺白石
	if( bc == 0 ) return false;
	for(;;) {
		ushort bc0 = bc;
		bc |= (bc0 << 4) & m_white;
		bc |= (bc0 << 3) & m_white & 0xeeee;
		bc |= (bc0 << 1) & m_white & 0xeeee;
		bc |= (bc0 >> 1) & m_white & 0x7777;
		bc |= (bc0 >> 3) & m_white & 0x7777;
		bc |= (bc0 >> 4) & m_white;
		if( bc == bc0 ) break;
	}
	return (bc & 0x1111) != 0;		//	右辺に達したか？
}
bool BitBoard44::playout_to_end(bool black_next) const {
	BitBoard44 b2(*this);
	ushort empty = ~(m_black | m_white);
	if( !empty ) return false;		//	空欄無し
	vector<ushort> lst;			//	空欄リスト
	while( empty != 0 ) {
		ushort b = empty & -empty;	//	最右ビットを取り出す
		lst.push_back(b);
		empty ^= b;
	}
	shuffle(lst.begin(), lst.end(), rgen);
	for(auto mask : lst) {
		if( black_next )
			b2.set_black(mask);
		else
			b2.set_white(mask);
		black_next = !black_next;
	}
	//b2.print();
	return b2.did_black_win();
}
double BitBoard44::playout_to_end(int N, bool black_next) const {
	int bwc = 0;	//	黒勝ち回数
	for(int i = 0; i < N; ++i) {
		if( playout_to_end(black_next) )
			++bwc;
	}
	return (double)bwc / N;
}
