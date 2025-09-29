#include <iostream>
#include <chrono>
#include <assert.h>
#include "Board.h"

using namespace std;

int main()
{
	if (1) {
		Board bd(4);
#if 0
		for (int i = 0; i != 4; ++i) {
			bd.set_color(bd.sel_move_itrdeep(BLACK), BLACK);
			bd.print();
			bd.set_color(bd.sel_move_itrdeep(WHITE), WHITE);
			bd.print();
		}
#endif
		//bd.set_color(4, 0, BLACK);
		//bd.set_color(2, 2, WHITE);
		//bd.set_color(0, 3, BLACK);
		//bd.set_color(1, 1, WHITE);
		//bd.set_color(4, 1, BLACK);
		//bd.set_color(2, 3, WHITE);
		//bd.set_color(2, 0, BLACK);
		//bd.set_color(1, 2, WHITE);
		//bd.set_color(0, 1, BLACK);
		//bd.set_color(0, 2, WHITE);
		//bd.set_color(0, 0, BLACK);
		Color next = bd.next_color();
		bd.print();
		int nemp = bd.n_empty();
		bd.clear_nodeSearched();
		auto start = std::chrono::high_resolution_clock::now();
		for(int y = 0; y < bd.get_width(); ++y) {
			cout << string(y, ' ');
			for(int x = 0; x < bd.get_width(); ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					int ix = bd.xyToIX(x, y);
					//bool b = bd.is_winning_move(ix, next, nemp);
					//bool b = bd.is_winning_move_FO(ix, next, nemp);
					//bool b = bd.is_winning_move_always_check(ix, next);
					//bool b = bd.is_winning_move_check_dist(ix, next);
					bool b = bd.is_winning_move_check_dist_FO(ix, next);
					if( next == WHITE ) b = !b;
					cout << (b?"B ":"W ");
				} else
					printf("- ");
			}
			cout << endl;
		}
		cout << endl;
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		cout << "duration: " << seconds*1000 << " msec" << endl;
		cout << "terminal node searched = " << bd.get_nodeSearched() << endl << endl;
	}
	if (0) {
		Board bd(8);
		//bd.set_color(5, 1, BLACK);
		//bd.set_color(3, 4, WHITE);
		//bd.set_color(5, 3, BLACK);
		//bd.set_color(1, 5, WHITE);
		//bd.set_color(5, 2, BLACK);
		byte next = BLACK;
		//next = WHITE;
		bd.print();
		auto ix = bd.sel_move_itrdeep(next, 1000);
		cout << "put " << bd.ixToStr(ix) << endl;
		//bd.print_tt(next);
    	bd.set_color(ix, next);
		bd.print();
	}
	if (0) {
		Board bd(6);
		bd.set_color(3, 2, BLACK);
		bd.set_color(2, 4, WHITE);
		bd.set_color(1, 4, BLACK);
		//bd.set_color(1, 0, WHITE);
		//bd.set_color(0, 0, BLACK);
		Color next = BLACK;
		next = WHITE;
		bd.print();
		for(int y = 0; y < bd.get_width(); ++y) {
			cout << string(y*3, ' ');
			for(int x = 0; x < bd.get_width(); ++x) {
				if( bd.get_color(x, y) == EMPTY ) {
					bd.set_color(x, y, next);
					//auto ev = -bd.eval((BLACK+WHITE)-next);
					auto ev = -bd.nega_max((BLACK+WHITE)-next, 3);
					bd.set_color(x, y, EMPTY);
					printf("%6.2f", ev);
				} else
					printf(" -----");
			}
			cout << endl;
		}
		cout << endl;
	}
	if (0) {
		Board bd(3);
		bd.set_color(1, 1, BLACK);
		bd.set_color(1, 0, WHITE);
		//bd.set_color(3, 1, BLACK);
		Color next = BLACK;
		//next = WHITE;
		bd.print();
	    int bvd = bd.calc_vert_dist(true);
	    int bhd = bd.calc_horz_dist(true);
	    //bd.print_dist();
	    cout << "bridged vert dist = " << bvd << endl;
	    cout << "bridged horz dist = " << bhd << endl << endl;
	    int vd = bd.calc_vert_dist();
	    //bd.print_dist();
	    int hd = bd.calc_horz_dist();
	    cout << "vert dist = " << vd << endl;
	    cout << "horz dist = " << hd << endl << endl;
		auto ev = bd.eval(next);
		cout << "ev = " << ev << endl;
	}
	if (0) {
		Board bd(3);
		Color next = BLACK;
		for (;;) {
			auto ev = bd.eval(next);
			cout << "ev = " << ev << endl;
			bd.print();
			int ix = bd.sel_move_random();
			if (ix < 0) break;
			bd.set_color(ix, next);
			bd.set_last_put_ix(ix);
			if( next == BLACK && bd.is_vert_connected() ||
				next == WHITE && bd.is_horz_connected() )
			{
				break;
			}
			next = (BLACK + WHITE) - next;
		}
		auto ev = bd.eval(next);
		cout << "ev = " << ev << endl;
		bd.print();
	}
	if( 0 ) {
	    Board bd(5);
	    Color next = BLACK;
	    for(int i = 0; i != 15; ++i) {
	    	int ix = bd.sel_move_random();
	    	bd.set_color(ix, next);
	    	next = (BLACK+WHITE)-next;
	    }
	    bd.print();
	    int bvd = bd.calc_vert_dist(true);
	    int bhd = bd.calc_horz_dist(true);
	    bd.print_dist();
	    cout << "bridged vert dist = " << bvd << endl;
	    cout << "bridged horz dist = " << bhd << endl << endl;
	    int vd = bd.calc_vert_dist();
	    //bd.print_dist();
	    int hd = bd.calc_horz_dist();
	    cout << "vert dist = " << vd << endl;
	    cout << "horz dist = " << hd << endl << endl;
	    if( 1 ) {
		    bd.swap_black_white();
		    bd.print();
		    int bvd = bd.calc_vert_dist(true);
		    //bd.print_dist();
		    int bhd = bd.calc_horz_dist(true);
		    bd.print_dist();
		    cout << "bridged vert dist = " << bvd << endl;
		    cout << "bridged horz dist = " << bhd << endl << endl;
		    int vd = bd.calc_vert_dist();
		    //bd.print_dist();
		    int hd = bd.calc_horz_dist();
		    cout << "vert dist = " << vd << endl;
		    cout << "horz dist = " << hd << endl << endl;
	    }
	}
	if( 0 ) {
	    Board bd(11);
	    int d = bd.do_itrdeep(BLACK, 1000);
		cout << "max depth = " << d << endl;
		cout << "nodeSearched = " << bd.get_nodeSearched() << endl;
		cout << "tt.size() = " << bd.get_tt_size() << endl << endl;
	}
	if( 0 ) {
	    Board bd(11);
		auto start = std::chrono::high_resolution_clock::now();
		bd.do_DFS(BLACK, 4);
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		cout << "duration: " << seconds*1000 << " msec" << std::endl << endl;
		cout << "nodeSearched = " << bd.get_nodeSearched() << endl << endl;
	}
	if( 0 ) {
	    Board bd(3);
	    bd.set_color(2, 0, BLACK);
	    bd.set_color(1, 1, BLACK);
	    bd.print();
	    int vbd = bd.calc_vert_dist(true);
	    bd.print_dist();
	    int hbd = bd.calc_horz_dist(true);
	    cout << "vert bridged dist = " << vbd << endl;
	    cout << "horz bridged dist = " << hbd << endl << endl;
	    int vd = bd.calc_vert_dist();
	    bd.print_dist();
	    int hd = bd.calc_horz_dist();
	    cout << "vert dist = " << vd << endl;
	    cout << "horz dist = " << hd << endl << endl;
	}
	if( 0 ) {
	    Board bd(6);
		const int LIMIT = 5000;
	    Color next = BLACK;
		int ix = 0;
		int last_ix = 0;
		int last2_ix = 0;
		for(;;) {
	    	if( next == BLACK ) {
		    	//ix = bd.sel_move_random();
				//ix = bd.sel_move_PMC(next, LIMIT);
				//ix = bd.sel_move_local_MC(next, ix, LIMIT);
				ix = bd.sel_move_itrdeep(next, LIMIT);
			} else {
		    	ix = bd.sel_move_PMC(next, LIMIT);
		    	//ix = bd.sel_move_local_MC(next, last_ix, last2_ix, LIMIT);
				//ix = bd.sel_move_itrdeep(next, LIMIT);
			}
	    	if( ix < 0 ) break;
	    	last2_ix = last_ix;
	    	last_ix = ix;
	    	bd.set_color(ix, next);
	    	bd.set_last_put_ix(ix);
	    	bd.print();
			if( next == BLACK && bd.is_vert_connected() ||
				next == WHITE && bd.is_horz_connected() )
			{
				break;
			}
			next = (BLACK+WHITE) - next;
	    }
	}
	if( 0 ) {
	    Board bd(4);
	    //bool bw = bd.playout_to_full(BLACK);		//	空欄が無くなるまで
	    bool bw = bd.playout_to_win(BLACK);			//	勝敗がつくまで
	    bd.print();
	    if( bw ) cout << "BLACK won." << endl;
	    else cout << "WHITE won." << endl;
	}
	if( 0 ) {
	    Board bd(11);
		auto start = std::chrono::high_resolution_clock::now();
		const int N_LOOP = 1000*1;
		int bwon = 0;
		for (int i = 0; i != N_LOOP; ++i) {
			bd.init();
			//bd.random_playout(BLACK);
			//if( bd.playout_to_full(BLACK) )		//	最後までプレイアウト＆勝敗判定
			if( bd.playout_to_win(BLACK) )		//	勝敗がつくまでプレイアウト
				bwon += 1;		//	黒勝ちの場合
			//bd.print();
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		cout << "N_LOOP = " << N_LOOP << endl;
		cout << "duration: " << seconds*1000 << " msec" << std::endl << endl;
		cout << "black won rate = " << bwon *100.0 / N_LOOP << "%" << endl << endl;
	    bd.print();
	}

    std::cout << "\nOK.\n";
}
