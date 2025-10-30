#include <iostream>
#include "HNetBoard.h"

using namespace std;

int main()
{
    if (1) {
        HNetBoard bd(3);
        vector<short> lst;
        bd.get_empty_indexes(lst);
		Color next = BLACK;
		for(int ix: lst) {
			bd.set_color(ix, next);
			bd.print();
			next = BLACK + WHITE - next;
		}
    }
    if (0) {
        HNetBoard bd(3);
        bd.set_color_update(2, 0, BLACK);
        bd.set_color_update(1, 2, WHITE);
        bd.set_color_update(0, 2, BLACK);
        bd.set_color_update(1, 1, BLACK);
        bd.print();
        bool bw = bd.did_win(BLACK);
        cout << "bw = " << bw << endl;
        auto vd = bd.calc_vert_distance();
        cout << "vd = " << vd << endl << endl;
        bd.print_dist();
        auto vh = bd.calc_horz_distance();
        cout << "vh = " << vh << endl << endl;
        bd.print_dist();
    }

    std::cout << "\nOK.\n";
}
