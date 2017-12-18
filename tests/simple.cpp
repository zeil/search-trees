#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <iostream>
#include <assert.h>

#include "two-three-tree.hpp"

using namespace search_trees;

static void visual_test()
{
	const std::string keys = "ALGORITHMS";

	auto tree = TwoThreeTree<char, int>::create();
	for (auto c : keys) {
		std::cout << "Insert '" << c << "':" << std::endl;
		tree->insert(c, 0);
		tree->print(std::cout);
	}

	for (auto c : keys) {
		std::cout << "Remove '" << c << "':" << std::endl;
		assert(tree->remove(c));
		tree->print(std::cout);
	}
}

static void big_test()
{
	const int nodes_count = 1024;

	auto tree = TwoThreeTree<int, int>::create();
	for (int i = 1; i <= nodes_count; ++i)
		tree->insert(i, i);

	for (int i = 1; i <= nodes_count; ++i) {
		auto found = tree->find(i);
		assert(found && *found == i);
	}

	for (int i = 1; i <= nodes_count; ++i) {
		assert(tree->remove(i));
		for (int j = 1; j <= i; ++j)
			assert(!tree->find(j));
		for (int j = i + 1; j <= nodes_count; ++j) {
			auto found = tree->find(j);
			assert(found && *found == j);
		}
	}
}

int main()
{
	visual_test();
	big_test();

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
