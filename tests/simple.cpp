#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <iostream>

#include "two-three-tree.hpp"

using namespace search_trees;

int main()
{
	auto tree1 = TwoThreeTree<char, int>::create();
	tree1->insert('A', 0);
	tree1->insert('L', 0);
	tree1->insert('G', 0);
	tree1->insert('O', 0);
	tree1->insert('R', 0);
	tree1->insert('I', 0);
	tree1->insert('T', 0);
	tree1->insert('H', 0);
	tree1->insert('M', 0);
	tree1->insert('S', 0);
	tree1->print(std::cout);

	auto tree2 = TwoThreeTree<int, int>::create();
	for (int i = 1; i <= 32; ++i)
		tree2->insert(i, 0);
	tree2->print(std::cout);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
