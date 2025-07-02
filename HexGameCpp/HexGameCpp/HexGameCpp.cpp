#include <iostream>
#include <assert.h>
#include "Board.h"

using namespace std;

const int BD_WIDTH = 4;

int main()
{
	Board bd(BD_WIDTH);
	if( true ) {
		cout << endl;
		bd.init();
		bd.set_color(2, 1, BLACK);
		bd.set_color(1, 3, WHITE);
		bd.set_color(0, 3, BLACK);
		bd.set_color(1, 2, WHITE);
		bd.set_color(0, 2, BLACK);
		bd.set_color(1, 1, WHITE);
		bd.set_color(0, 1, BLACK);
		bd.set_color(1, 0, WHITE);
		bd.print();
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*2, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, BLACK);
					//auto ev = bd.eval();
					auto ev = bd.white_turn(3);
					//cout << "ev = " << ev << endl;
					//bd.print();
					bd.set_color(x, y, EMPTY);
					ev = min(999, ev);
					printf("%4d", ev);
				} else
					printf("   *");
			}
			cout << endl;
		}
		cout << endl;
	}
	if( false ) {
		cout << endl;
		bd.init();
		bd.set_color(1, 1, BLACK);
		bd.set_color(0, 1, WHITE);
		bd.print();
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, BLACK);
					auto ev = bd.eval();
					//cout << "ev = " << ev << endl;
					//bd.print();
					bd.set_color(x, y, EMPTY);
					printf("%2d", ev);
				} else
					printf(" -");
			}
			cout << endl;
		}
		cout << endl;
	}
	if( false ) {
		bd.init();
		bd.set_color(1, 1, BLACK);
		bd.set_color(0, 1, WHITE);
		bd.set_color(2, 1, BLACK);
		//bd.set_color(3, 1, BLACK);
		//bd.set_color(0, 2, BLACK);
		//bd.set_color(1, 1, BLACK);
		//assert( bd.get_color(1, 1) ==  BLACK );
		//bd.set_color(0, 2, BLACK);
		//bd.set_color(2, 2, WHITE);
		//assert( bd.get_color(1, 2) ==  WHITE );
		//bd.set_color(0, 3, WHITE);
		bd.print();
		auto dv = bd.calc_vert_dist();
		cout << "vertical dist = " << dv << endl;
		auto dh = bd.calc_horz_dist();
		cout << "horizontal dist = " << dh << endl;
		//auto ev = bd.eval();
	}
#if 0
	if( false ) {
		bd.init();
		int mxev = -9999;
		int bestix = -1;
		for(int y = 0; y < BD_WIDTH; ++y) {
			for(int x = 0; x < BD_WIDTH; ++x) {
				bd.set_color(x, y, BLACK);
				auto ev = bd.eval(WHITE);
				cout << "ev = " << ev << endl;
				bd.print();
				bd.set_color(x, y, EMPTY);
				//printf("%3d", ev);
			}
			//cout << endl;
		}
		//bd.print();
	}
#endif

    std::cout << "\nOK.\n";
}
