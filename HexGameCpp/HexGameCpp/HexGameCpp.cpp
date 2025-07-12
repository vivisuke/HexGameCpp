#include <iostream>
#include <assert.h>
#include "Board.h"

using namespace std;

const int BD_WIDTH = 5;

int main()
{
	cout << endl;
	Board bd(BD_WIDTH);
	if( false ) {
		bd.init();
		bd.set_color(1, 1, BLACK);
		Board bd2(bd);
		bd2.print();
	}
	if( true ) {
		bd.init();
		//byte next = BLACK;
		bd.set_color(1, 1, BLACK);
		byte next = WHITE;
		byte n2 = (BLACK+WHITE) - next;
		bd.print();
		const int N_PLAYOUT = 1000;
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*2, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, next);
					auto rate = bd.estimate_win_rate_PMC(n2, N_PLAYOUT);
					bd.set_color(x, y, EMPTY);
					printf(" %.2f", 1 - rate);
				} else
					printf("    -");
			}
			cout << endl;
		}
		cout << endl;
	}
	if( false ) {
		int N_LOOP = 1000;
		int n_black_won = 0;
		for(int c = 0; c < N_LOOP; ++c) {
			bd.init();
			//bd.set_color(1, 2, BLACK);
			byte next = BLACK;
			//byte next = WHITE;
			auto b = bd.playout(next);
			if( b ) {
				cout << "BLACK win." << endl;
				++n_black_won;
			} else {
				cout << "WHITE win." << endl;
			}
		}
		cout << "n_black_won = " << n_black_won << " / " << N_LOOP << endl;
	}
	if( false ) {
		int N_LOOP = 1000;
		int n_black_won = 0;
		for(int c = 0; c < N_LOOP; ++c) {
			bd.init();
			//bd.set_color(1, 2, BLACK);
			byte next = BLACK;
			//byte next = WHITE;
			for(;;) {
				if (0) {
					bd.print();
					auto dv = bd.calc_vert_dist(false);
					cout << "vertical dist = " << dv << endl;
					auto dh = bd.calc_horz_dist(false);
					cout << "horizontal dist = " << dh << endl;
				}
				int ix = bd.sel_move_random();
				if( ix < 0 ) break;
				bd.set_color(ix, next);
				if( next == BLACK ) {
					auto dv = bd.calc_vert_dist(true);
					if( dv == 0 ) {
						bd.print();
						cout << "BLACK win." << endl;
						++n_black_won;
						break;
					}
				} else {
					auto dh = bd.calc_horz_dist(true);
					if( dh == 0 ) {
						bd.print();
						cout << "WHITE win." << endl;
						break;
					}
				}
				next = (BLACK + WHITE) - next;
			}
			cout << endl;
		}
		cout << "n_black_won = " << n_black_won << " / " << N_LOOP << endl;
	}
	if( false ) {
		bd.init();
		//bd.set_color(2, 1, BLACK);
		//bd.set_color(1, 3, WHITE);
		//bd.set_color(0, 3, BLACK);
		//bd.set_color(1, 2, WHITE);
		//bd.set_color(0, 2, BLACK);
		//bd.set_color(1, 1, WHITE);
		//bd.set_color(0, 1, BLACK);
		//bd.set_color(1, 0, WHITE);
		bd.print();
		const int DEPTH = 6;		//	このレベルも含めての先読み手数
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*2, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, BLACK);
					//auto ev = bd.eval();
					auto ev = bd.white_turn(DEPTH-1);
					//cout << "ev = " << ev << endl;
					//bd.print();
					bd.set_color(x, y, EMPTY);
					ev = max(min(ev, 99), -99);
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
		bd.set_color(2, 1, BLACK);
		bd.print();
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*2, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, WHITE);
					//auto ev = bd.eval();
					auto ev = bd.black_turn(3);
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
#if 1
		bd.set_color(1, 2, BLACK);
		bd.set_color(2, 0, WHITE);
		bd.set_color(0, 1, BLACK);
		bd.set_color(1, 1, WHITE);
#else
		bd.set_color(3, 0, BLACK);
		bd.set_color(2, 2, WHITE);
		bd.set_color(1, 2, BLACK);
		bd.set_color(2, 1, WHITE);
#endif
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
