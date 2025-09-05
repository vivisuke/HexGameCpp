#include <iostream>
#include <chrono>
#include <assert.h>
#include "Board.h"

using namespace std;

int main()
{
	if( 0 ) {
	    Board bd(4);
	    //bool bw = bd.playout_to_full(BLACK);		//	空欄が無くなるまで
	    bool bw = bd.playout_to_win(BLACK);			//	勝敗がつくまで
	    bd.print();
	    if( bw ) cout << "BLACK won." << endl;
	    else cout << "WHITE won." << endl;
	}
	if( 1 ) {
	    Board bd(11);
		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0; i != 1000000; ++i) {
			bd.init();
			//bd.random_playout(BLACK);
			//bd.playout_to_full(BLACK);		//	最後までプレイアウト＆勝敗判定
			bd.playout_to_win(BLACK);		//	勝敗がつくまでプレイアウト
			//bd.print();
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = end - start;
		double seconds = std::chrono::duration<double>(duration).count();
		std::cout << "duration: " << seconds*1000 << " msec" << std::endl << endl;
	    bd.print();
	}

    std::cout << "\nOK.\n";
}
