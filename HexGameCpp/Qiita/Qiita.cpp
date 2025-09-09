#include <iostream>
#include <chrono>
#include <assert.h>
#include "Board.h"

using namespace std;

int main()
{
	if( 1 ) {
	    Board bd(11);
	    int d = bd.do_itrdeep(BLACK, 1000);
		cout << "max depth = " << d << endl;
		cout << "nodeSearched = " << bd.get_nodeSearched() << endl << endl;
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
	    Board bd(4);
	    Color next = BLACK;
	    for(;;) {
	    	int ix;
	    	if( next == BLACK ) {
		    	//ix = bd.sel_move_random();
				ix = bd.sel_move_PMC(next, 1000);
			} else {
		    	ix = bd.sel_move_PMC(next, 1000);
	    	}
	    	if( ix < 0 ) break;
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
