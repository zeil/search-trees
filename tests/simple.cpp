#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <functional>
#include <iostream>
#include <fstream>
#include <assert.h>

#include "two-three-tree.hpp"
#include "red-black-tree.hpp"

using namespace search_trees;

template<typename Key, typename Value>
using SearchTreeFactory = std::function<SearchTreePtr<Key, Value>(void)>;

static void visual_test(SearchTreeFactory<char, int> factory, std::ostream &stream)
{
	const std::string keys = "ALGORITHMS";

	auto tree = factory();
	for (auto c : keys) {
		stream << "Insert '" << c << "':" << '\n';
		tree->insert(c, 0);
		tree->print(stream);
	}

	for (auto c : keys) {
		stream << "Remove '" << c << "':" << '\n';
		assert(tree->remove(c));
		tree->print(stream);
	}
}

static void big_test(SearchTreeFactory<int, int> factory)
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
	//std::ofstream file("test.txt", std::ofstream::out);
	//std::ostream &stream = file;
	//std::ostream &stream = std::cout;

	//SearchTreeFactory<char, int> factory = TwoThreeTree<char, int>::create;
	//SearchTreeFactory<char, int> factory = RedBlackTree<char, int>::create;
	//visual_test(factory, stream);

	SearchTreeFactory<int, int> factory = TwoThreeTree<int, int>::create;
	big_test(factory);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
