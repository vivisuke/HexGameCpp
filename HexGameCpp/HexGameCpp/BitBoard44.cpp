#include <iostream>
#include <string>
#include "BitBoard44.h"

using namespace std;

const int BD_WIDTH = 4;
const int BD_HEIGHT = 4;

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
				cout << "œ";
			else if( (m_white & mask) != 0 )
				cout << "ü";
			else
				cout << "E";
			mask >>= 1;
		}
		cout << endl;
	}
	cout << endl;
}
