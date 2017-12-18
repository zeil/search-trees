#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <iostream>
#include <assert.h>

#include "two-three-tree.hpp"

using namespace search_trees;

int main()
{
	const std::string keys = "ALGORITHMS";

	auto tree1 = TwoThreeTree<char, int>::create();
	for (auto c: keys)
		tree1->insert(c, 0);
	tree1->print(std::cout);

	tree1->remove('A');
	tree1->remove('L');
	tree1->remove('G');
	tree1->remove('O');
	tree1->print(std::cout);

	// for (auto c: keys) {
	// 	tree1->remove(c);
	// 	tree1->print(std::cout);
	// }

	// auto tree2 = TwoThreeTree<int, int>::create();
	// const int nodes_count = 32;
	// for (int i = 1; i <= nodes_count; ++i)
	// 	tree2->insert(i, i);
	// tree2->print(std::cout);

	// for (int i = 1; i <= nodes_count; ++i) {
	// 	auto found = tree2->find(i);
	// 	assert(found && *found == i);
	// }
	// auto found = tree2->find(nodes_count + 1);
	// assert(!found);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
