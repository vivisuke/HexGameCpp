#include <iostream>
#include <assert.h>
#include "Board.h"

using namespace std;

int main()
{
	Board bd(4);
	bd.set_color(1, 1, BLACK);
	assert( bd.get_color(1, 1) ==  BLACK );
	bd.set_color(1, 2, WHITE);
	assert( bd.get_color(1, 2) ==  WHITE );
	bd.print();

    std::cout << "\nOK.\n";
}
