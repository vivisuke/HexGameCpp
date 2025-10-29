#include <iostream>
#include "HNetBoard.h"

using namespace std;

int main()
{
    HNetBoard bd(3);
    bd.set_color_update(1, 1, BLACK);
    bd.print();
    auto vd = bd.calc_vert_distance();
    cout << "vd = " << vd << endl << endl;
    bd.print_dist();

    std::cout << "\nOK.\n";
}
