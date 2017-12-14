#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <iostream>

#include "two-three-tree.hpp"

using namespace search_trees;

int main()
{
	std::unique_ptr<SearchTree<char, int>> tree1 = std::make_unique<TwoThreeTree<char, int>>();
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

	std::unique_ptr<SearchTree<char, int>> tree2 = std::make_unique<TwoThreeTree<char, int>>();
	for (char c = 'A'; c <= 'Z'; ++c)
		tree2->insert(c, 0);
	tree2->print(std::cout);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
