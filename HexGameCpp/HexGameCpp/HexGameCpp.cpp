#include <iostream>
#include <random>
#include <chrono>
#include <assert.h>
#include <bit>
#include <intrin.h>
#include "Board.h"
#include "BitBoard4.h"
#include "BitBoard8.h"

using namespace std;

//extern std::mt19937 rgen; 

const int BD_WIDTH = 4;

//int my_popcount(ushort b) {
//
//}

void print_dvdh(Board& bd) {
	bd.print();
	auto dv = bd.calc_vert_dist();
	cout << "vertical dist = " << dv << endl;
	auto dh = bd.calc_horz_dist();
	cout << "horizontal dist = " << dh << endl;
}
void print_dvdh2(Board& bd) {
	//bd.print();
	auto dv = bd.calc_vert_dist();
	//bd.print_dist();
	auto dv2 = bd.calc_vert_dist(false);
	cout << "vert dist = " << dv << ", " << dv2 << endl;
	auto dh = bd.calc_horz_dist();
	auto dh2 = bd.calc_horz_dist(false);
	cout << "horz dist = " << dh << ", " << dh2 << endl << endl;
}
string ix_str(const Board& bd, int ix) {
	string txt(1, 'a'+bd.indexToX(ix));
	txt +=string(1,  '1' + bd.indexToY(ix));
	return txt;
}

int main()
{
	const int N_PLAYOUT = 1000;
	if (1) {
#if 0
		const int BD_WIDTH = 2;
		Board bd(BD_WIDTH);
		//bd.calc_horz_dist();
		//bd.print_dist();
		//bd.calc_horz_dist(true, false);
		//bd.print_dist();
		bd.set_color(0, 0, BLACK);
		bd.set_color(0, 1, WHITE);
#elif 1
		const int BD_WIDTH = 3;
		Board bd(BD_WIDTH);
		bd.set_color(2, 0, BLACK);
		//bd.set_color(1, 2, WHITE);
#else
		const int BD_WIDTH = 4;
		Board bd(BD_WIDTH);
		bd.set_color(1, 2, BLACK);
		bd.set_color(2, 0, WHITE);
		//bd.set_color(0, 1, BLACK);
		//bd.set_color(1, 0, WHITE);
#endif
		bd.print();
		//bd.calc_horz_dist();
		//bd.print_dist();
		//bd.calc_horz_dist(true, false);
		//bd.print_dist();
		print_dvdh2(bd);
		cout << "black: eval = " << bd.eval_black() << endl << endl;
		cout << "white: eval = " << bd.eval_white() << endl << endl;
	}
	if (0) {
		Board bd(4);
		bd.print();
		auto ix = bd.sel_move_win(BLACK);
		cout << "move_win: ";
		if (ix < 0) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(2, 1, BLACK);
		bd.print();
		ix = bd.sel_move_win(WHITE);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(WHITE);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(2, 0, WHITE);
		bd.print();
		ix = bd.sel_move_win(BLACK);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(BLACK);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(3, 0, BLACK);
		bd.print();
		ix = bd.sel_move_win(WHITE);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(WHITE);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

	}
	if (0) {
		Board bd(3);
		bd.print();
		auto ix = bd.sel_move_win(BLACK);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(2, 1, BLACK);
		bd.print();
		ix = bd.sel_move_win(WHITE);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(WHITE);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(2, 0, WHITE);
		bd.print();
		ix = bd.sel_move_win(BLACK);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(BLACK);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(0, 1, BLACK);
		bd.print();
		ix = bd.sel_move_win(WHITE);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(WHITE);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(0, 0, WHITE);
		bd.print();
		ix = bd.sel_move_win(BLACK);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(BLACK);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;

		bd.set_color(1, 0, BLACK);
		bd.print();
		ix = bd.sel_move_win(WHITE);
		cout << "move_win: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		ix = bd.sel_move_block(WHITE);
		cout << "move_block: ";
		if( ix < 0 ) cout << "none" << endl;
		else cout << ix_str(bd, ix) << endl;
		cout << endl;
	}
	if (0) {
		Board bd(3);
		auto ix = bd.sel_move_MCTS(BLACK);
		printf("best: %c%d\n", 'a'+bd.indexToX(ix), bd.indexToY(ix)+1);
		//cout << "best: " << bd.indexToX(ix) << ", " << bd.indexToY(ix) << endl;
	}
	if (0) {
		Board bd(4);
		bd.print();
		byte next = BLACK;
		for(;;) {
			int ix;
			if (next == BLACK) {
				//ix = bd.sel_move_random();
				ix = bd.sel_move_PMC(next);
				//ix = bd.sel_move_MCTS(next);
			} else {
				//ix = bd.sel_move_random();
				//ix = bd.sel_move_PMC(next);
				ix = bd.sel_move_MCTS(next);
			}
			if( ix < 0 ) break;
			if( bd.put_and_check_uf(ix, next) )
				break;
			bd.print();
			next = (BLACK + WHITE) - next;
		}
		bd.print();
	}
	if (0) {
		Board bd(4);
		//bd.put_and_check_uf(bd.xyToIndex(1, 1), BLACK);
		//bd.put_and_check_uf(bd.xyToIndex(0, 0), WHITE);
		bd.print();
		double p = bd.playout_smart(1000, BLACK);
		cout << "black win rate = " << p << endl;
	}
	if (0) {
		Board bd(4);
		auto w = bd.playout_smart(BLACK);
		bd.print();
		if( w == BLACK ) cout << "BLACK won." << endl;
		else cout << "WHITE won." << endl;
	}
	if (0) {
		Board bd(3);
		bool b = bd.put_and_check_uf(bd.xyToIndex(1, 0), BLACK);
		b = bd.put_and_check_uf(bd.xyToIndex(1, 1), BLACK);
		b = bd.put_and_check_uf(bd.xyToIndex(1, 2), BLACK);
		bd.print();
	}
	if( 0 ) {
		auto start = std::chrono::high_resolution_clock::now();
		for(int i = 0; i < N_PLAYOUT; ++i) {
			BitBoard8 b8;
			//b8.set_black(2, 1);
			b8.playout(true);
			//b8.print();
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds*1000 << " msec" << std::endl << endl;
	}
	if (0) {
		Board bd(8);
		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i < N_PLAYOUT; ++i) {
			Board b8(8);
			//b8.set_black(2, 1);
			b8.playout_to_end(true);
			//b8.print();
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds * 1000 << " msec" << std::endl << endl;
		//bd.playout_to_end(true);
		//bd.print();
	}
	if( 0 ) {
		auto start = std::chrono::high_resolution_clock::now();
		for(int i = 0; i < 1000*1000; ++i) {
			//auto pc = std::popcount(0x123);
			int r = rand() % 16;
			//auto pc = __popcnt(r);
			int m = 1 << r;
			int r2 = __popcnt(m-1);
			if( r2 != r ) {
				cout << "???" << endl;
				break;
			}
			//cout << "popcount = " << pc << endl;
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds*1000 << " msec" << std::endl;
	}
	cout << endl;
	if (0) {
		const int BD_WIDTH = 3;
		Board bd(BD_WIDTH);
		bd.init();
		bd.put_and_check_uf(bd.xyToIndex(2, 0), BLACK);
		bd.print();
		auto start = std::chrono::high_resolution_clock::now();
		double maxv = -9999;
		double minv = 9999;
		//bool next = true;	//	true for 黒番
		byte next = WHITE;
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*3, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					//bd.set_color(x, y, next);
					bd.put_and_check_uf(bd.xyToIndex(x, y), next);
					double r = bd.playout_smart(100000, (BLACK+WHITE)-next);
					maxv = max(maxv, r);
					minv = min(minv, r);
					printf(" %.3f", r);
					bd.set_empty(x, y);
				} else
					cout << " ---";
			}
			cout << endl;
		}
		cout << endl;
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds*1000 << " msec" << std::endl;
		//cout << "maxv = " << maxv << endl;
		//cout << "minv = " << minv << endl;
		printf("maxv = %.3f\n", maxv);
		printf("minv = %.3f\n", minv);
	}
	BitBoard4 b4;
	if (0) {
		b4.init();
		b4.set_black(3, 0);
		b4.print();
		auto start = std::chrono::high_resolution_clock::now();
		double maxv = -9999;
		double minv = 9999;
		bool next = false;	//	true for 黒番
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*3, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( b4.get_color(x, y) == EMPTY ) {
					b4.set_color(x, y, next);
					//double r = b4.playout_to_end(100000, !next);
					//double r = b4.playout_smart(100000, !next);
					double r = b4.PMC_score(100000, !next);
					maxv = max(maxv, r);
					minv = min(minv, r);
					printf(" %.3f", r);
					b4.set_empty(x, y);
				} else
					cout << " ---";
			}
			cout << endl;
		}
		cout << endl;
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds*1000 << " msec" << std::endl;
		//cout << "maxv = " << maxv << endl;
		//cout << "minv = " << minv << endl;
		printf("maxv = %.3f\n", maxv);
		printf("minv = %.3f\n", minv);
	}
	if (0) {
		b4.init();
		//auto b = b4.playout_to_end(true);
		auto b = b4.playout_smart(true, true);
		//b4.print();
		cout << (b ? "black won" : "white won") << endl;
	}
	if( 0 ) {
		b4.init();
		b4.set_black(1, 2);
		b4.set_white(2, 0);
		b4.print();
		cout << (b4.did_black_win() ? "black won" : "not black won") << endl;
		b4.set_black(1, 0);
		b4.set_black(1, 1);
		b4.set_black(1, 3);
		b4.print();
		cout << (b4.did_black_win() ? "black won" : "white won") << endl;
	}
	Board bd(BD_WIDTH);
	if( 0 ) {	//	dfs_black_win() テスト
		bd.init();
		//bd.set_color(2, 0, BLACK);
		//bd.set_color(1, 2, WHITE);
		//bd.set_color(0, 2, BLACK);
		//bd.set_color(1, 1, WHITE);
		//bd.set_color(0, 1, BLACK);
		//bd.set_color(1, 0, WHITE);
		bd.print();
		byte next = BLACK;
		//byte next = WHITE;
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, next);
					auto b = dfs_black_win(bd, (BLACK+WHITE)-next);
					if( b ) {
						printf(" B");
					} else {
						printf(" W");
					}
					bd.set_color(x, y, EMPTY);
				} else
					printf(" #");
			}
			cout << endl;
		}
		cout << endl;
	}
	if( 0 ) {	//	playout_rave() テスト
		bd.init();
		//bd.set_color(1, 2, BLACK);
		//bd.set_color(2, 0, WHITE);
		//bd.set_color(2, 1, BLACK);
		//bd.set_color(3, 0, WHITE);
		//bd.set_color(0, 1, BLACK);
		//bd.set_color(1, 0, WHITE);
		bd.print();
		for(int i = 0; i <= 1000; ++i)
			bd.playout_rave(BLACK);
		for(int y = 0; y < BD_WIDTH; ++y) {
			cout << string(y*2, ' ');
			for(int x = 0; x < BD_WIDTH; ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					auto rave = bd.m_rave[bd.xyToIndex(x, y)];
					printf("%4d", rave);
				} else
					printf("   #");
			}
			cout << endl;
		}
		cout << endl;
	}
	if( 0 ) {
		bd.init();
		auto start = std::chrono::high_resolution_clock::now();

		//bd.playout_old(BLACK);
		auto b = bd.playout(BLACK);

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds*1000 << " msec" << std::endl;
	}
	if( 0 ) {
		Board bd(4);
		bd.init();
		//bd.set_color(2, 3, BLACK);
		//bd.set_color(1, 5, WHITE);
		//bd.set_color(3, 4, BLACK);
		//bd.set_color(3, 1, WHITE);
		byte next = BLACK;
		//byte next = WHITE;
		bd.print();
		for(;;) {
			int ix = bd.sel_move_PMC(next);
			if( ix < 0 ) break;
			bd.set_color(ix, next);
			bd.print();
			if( next == BLACK ) {
				auto dv = bd.calc_vert_dist(false);
				if( dv == 0 ) {
					//bd.print();
					cout << "BLACK win." << endl;
					break;
				}
			} else {
				auto dh = bd.calc_horz_dist(false);
				if( dh == 0 ) {
					//bd.print();
					cout << "WHITE win." << endl;
					break;
				}
			}
			next = (BLACK + WHITE) - next;
		}
		cout << endl;
	}
	if( false ) {
		bd.init();
		bd.set_color(1, 1, BLACK);
		Board bd2(bd);
		bd2.print();
	}
	if( false ) {
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
					auto ev = bd.eval_black();
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
	if( 0 ) {
		Board bd(3);
		bd.init();
		print_dvdh(bd);
		bd.set_color(2, 1, BLACK);
		print_dvdh(bd);
		bd.set_color(2, 0, BLACK);
		print_dvdh(bd);
		bd.set_color(2, 0, EMPTY);
		bd.set_color(2, 2, BLACK);
		print_dvdh(bd);
	}
	if( 0 ) {
		Board bd(4);
		bd.init();
#if 1
		bd.set_color(1, 2, BLACK);
		//bd.set_color(2, 0, WHITE);
		//bd.set_color(0, 1, BLACK);
		//bd.set_color(1, 1, WHITE);
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
		print_dvdh(bd);
		bd.set_color(2, 0, WHITE);
		print_dvdh(bd);
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
