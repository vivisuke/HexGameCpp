#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "BitBoard8.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 

void BitBoard8::print() const {
	cout << "   ";
	for(int x = 0; x < BD_WIDTH; ++x)
		printf("%c ", 'a'+x);
	cout << endl;
	for(int y = 0; y < BD_HEIGHT; ++y) {
		cout << string(y, ' ');
		printf("%2d:", y+1);
		for(int x = 0; x < BD_WIDTH; ++x) {
			switch( get_color(x, y) ) {
			case BLACK: cout << "●"; break;
			case WHITE: cout << "◯"; break;
			default:
				cout << "・";
			}
		}
		cout << endl;
	}
	cout << endl;
}
byte BitBoard8::get_color(uint64 mask) const {
	if( (m_black & mask) != 0 ) return BLACK;
	if( (m_white & mask) != 0 ) return WHITE;
	return EMPTY;
}
void BitBoard8::playout(bool black_next) {
	uint64 empty = ~(m_black | m_white);
	if( !empty ) return;		//	空欄無し
	vector<uint64> lst;			//	空欄リスト
	while( empty != 0 ) {
		uint64 b = empty & -(int64)empty;	//	最右ビットを取り出す
		lst.push_back(b);
		empty ^= b;
	}
	shuffle(lst.begin(), lst.end(), rgen);		//	着手箇所をシャフル
	for(auto mask : lst) {
		if( black_next )
			set_black(mask);
		else
			set_white(mask);
		black_next = !black_next;
	}
}
