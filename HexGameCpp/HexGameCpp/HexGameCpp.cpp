#include <iostream>
#include <assert.h>
#include "Board.h"

using namespace std;

const int BD_WIDTH = 3;

int main()
{
	Board bd(BD_WIDTH);
	if( true ) {
		bd.init();
		//bd.set_color(3, 1, BLACK);
		//bd.set_color(0, 2, BLACK);
		bd.set_color(1, 1, BLACK);
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
		//auto ev = bd.eval(WHITE);
	}
	if( false ) {
		bd.init();
		for(int y = 0; y < BD_WIDTH; ++y) {
			for(int x = 0; x < BD_WIDTH; ++x) {
				bd.set_color(x, y, BLACK);
				auto ev = bd.eval(WHITE);
				//cout << "ev = " << ev << endl;
				//bd.print();
				bd.set_color(x, y, EMPTY);
				printf("%3d", ev);
			}
			cout << endl;
		}
		bd.print();
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
