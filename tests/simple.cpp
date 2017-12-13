#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <iostream>

#include "two-three-tree.hpp"

int main()
{
	TwoThreeTree<char, int> tree;
	tree.insert('A', 0);
	tree.insert('L', 0);
	tree.insert('G', 0);
	tree.insert('O', 0);
	tree.insert('R', 0);
	tree.insert('I', 0);
	tree.insert('T', 0);
	tree.insert('H', 0);
	tree.insert('M', 0);
	tree.insert('S', 0);
	tree.print(std::cout);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
