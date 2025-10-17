#include <iostream>
#include <random>
#include <chrono>
#include <assert.h>
#include "Board.h"

using namespace std;

static std::random_device rd;
static std::mt19937 rgen(rd()); 

int main()
{
	if (0) {
		Board bd(3);
		bd.print();
		bd.print_parent_ul();
		bd.set_color_uf(2, 0, BLACK);
		//bd.union_find(bd.xyToIX(2, 0), BLACK);
		bd.set_color_uf(0, 1, BLACK);
		bd.set_color_uf(1, 1, BLACK);
		bd.print();
		bd.print_parent_ul();
		bd.set_color(1, 1, EMPTY);
		bd.set_color(0, 1, EMPTY);
		bd.set_color(2, 0, EMPTY);
		bd.undo_union_find();
		bd.undo_union_find();
		bd.undo_union_find();
		bd.print();
		bd.print_parent_ul();
	}
	if (0) {
		Board bd(4);
		bd.set_color(0, 2, BLACK);
		//bd.set_color(1, 2, BLACK);
		//bd.set_color(2, 3, BLACK);
		bd.set_color(1, 1, WHITE);
		bd.set_color(3, 0, WHITE);
		bd.print();
		auto v = bd.is_vert_connected_v();
		cout << "bd.is_vert_connected() = " << v << endl;
		auto h = bd.is_horz_connected_v();
		cout << "bd.is_horz_connected() = " << h << endl;
	}
	if (1) {		//	勝敗が決するまでランダムに打つ
		Board bd(4);
		bd.print();
		std::vector<int> lst;
		bd.get_empty_indexes(lst);
		auto start = std::chrono::high_resolution_clock::now();
		int bwon = 0;		//	黒勝利回数
		int wwon = 0;
		const int N_LOOP = 1000;
		for(int i = 0; i < N_LOOP; ++i) {
			bd.init();
			shuffle(lst.begin(), lst.end(), rgen);
			Color next = BLACK;
			for(int ix: lst) {
				bd.set_color(ix, next);
				bd.set_last_put_ix(ix);
				//bd.print();
#if 0
				if( bd.union_find(ix, next) ) {
					if( next == BLACK ) {
						++bwon;
					} else {
						++wwon;
					}
					break;
				}
#else
				if( next == BLACK && bd.is_vert_connected_v() ) {
					++bwon;
					break;
				} else if( next == WHITE && bd.is_horz_connected_v() ) {
					++wwon;
					break;
				}
#endif
				//assert( !bd.union_find(ix, next) );
				next = BLACK + WHITE - next;
			}
		}
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
		bd.print();
        cout << "BLACK won: " << bwon << ", WHITE won: " << wwon << endl;
        cout << "duration = " << duration << "msec" << endl;
	}
	if (0) {
		Board bd(8);
		bd.print();
		std::vector<int> lst;
		bd.get_empty_indexes(lst);
		shuffle(lst.begin(), lst.end(), rgen);
		Color next = BLACK;
		for(int ix: lst) {
			bd.set_color(ix, next);
			bd.set_last_put_ix(ix);
			bd.print();
			if( next == BLACK && bd.is_vert_connected() ||
				next == WHITE && bd.is_horz_connected() )
			{
				assert( bd.union_find(ix, next) );
				break;
			}
			assert( !bd.union_find(ix, next) );
			next = BLACK + WHITE - next;
		}
	}
    cout << "\nOK.\n";
}
