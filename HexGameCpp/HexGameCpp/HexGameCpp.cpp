#include <iostream>
#include <assert.h>
#include "Board.h"

using namespace std;

int main()
{
	Board bd(4);
	bd.set_color(1, 1, BLACK);
	assert( bd.get_color(1, 1) ==  BLACK );
	bd.set_color(0, 2, BLACK);
	bd.set_color(1, 2, WHITE);
	assert( bd.get_color(1, 2) ==  WHITE );
	bd.set_color(0, 3, WHITE);
	bd.print();
	auto dv = bd.calc_vert_dist();
	cout << "vertical dist = " << dv << endl;
	auto dh = bd.calc_horz_dist();
	cout << "horizontal dist = " << dh << endl;

    std::cout << "\nOK.\n";
}
