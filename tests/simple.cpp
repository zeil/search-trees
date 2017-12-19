#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <functional>
#include <iostream>
#include <assert.h>

#include "two-three-tree.hpp"

using namespace search_trees;

template<typename Key, typename Value>
using SearchTreeFactory = std::function<SearchTreePtr<Key, Value>(void)>;

template<typename Key, typename Value>
static void visual_test(SearchTreeFactory<Key, Value> factory)
{
	const std::string keys = "ALGORITHMS";

	auto tree = factory();
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

template<typename Key, typename Value>
static void big_test(SearchTreeFactory<Key, Value> factory)
{
	const int nodes_count = 1024;

	auto tree = factory();
	for (int i = 1; i <= nodes_count; ++i)
		tree->insert(i, 2 * i);

	for (int i = 1; i <= nodes_count; ++i) {
		auto found = tree->find(i);
		assert(found && *found == 2 * i);
	}

	for (int i = 1; i <= nodes_count; ++i) {
		assert(tree->remove(i));
		for (int j = 1; j <= i; ++j)
			assert(!tree->find(j));
		for (int j = i + 1; j <= nodes_count; ++j) {
			auto found = tree->find(j);
			assert(found && *found == 2 * j);
		}
	}
}

int main()
{
	SearchTreeFactory<int, int> factory = TwoThreeTree<int, int>::create;
	visual_test(factory);
	big_test(factory);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
