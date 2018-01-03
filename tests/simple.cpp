#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#endif

#include <functional>
#include <iostream>
#include <chrono>
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

static void big_test(SearchTreeFactory<int, int> factory, std::ostream &stream, bool check_remove = false)
{
	const int nodes_count = 1024 * 1024;

	auto start = std::chrono::high_resolution_clock::now();

	auto tree = factory();
	for (int i = 1; i <= nodes_count; ++i)
		tree->insert(i, 2 * i);

	auto finish = std::chrono::high_resolution_clock::now();
	stream << "Creation of tree with " << nodes_count << " nodes took " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << " ms\n";

	start = std::chrono::high_resolution_clock::now();

	for (int i = 1; i <= nodes_count; ++i) {
		auto found = tree->find(i);
		assert(found && *found == 2 * i);
	}

	finish = std::chrono::high_resolution_clock::now();
	stream << "Finding all nodes took " << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() << " ms\n";

	auto min = tree->min();
	assert(min && *min == 2);
	auto max = tree->max();
	assert(max && *max == 2 * nodes_count);

	if (check_remove) {
		for (int i = 1; i <= nodes_count; ++i) {
			assert(tree->remove(i));
			for (int j = 1; j <= i; ++j)
				assert(!tree->find(j));
			for (int j = i + 1; j <= nodes_count; ++j) {
				auto found = tree->find(j);
				assert(found && *found == 2 * j);
			}
			if (i < nodes_count) {
				min = tree->min();
				assert(min && *min == 2 * (i + 1));
				max = tree->max();
				assert(max && *max == 2 * nodes_count);
			}
		}
	}
}

int main()
{
	std::ostream &stream = std::cout;

	SearchTreeFactory<char, int> char_factory = TwoThreeTree<char, int>::create;
	SearchTreeFactory<int, int> int_factory = TwoThreeTree<int, int>::create;
	stream << "2-3 tree:\n";
	//visual_test(char_factory, stream);
	big_test(int_factory, stream);

	char_factory = RedBlackTree<char, int>::create;
	int_factory = RedBlackTree<int, int>::create;
	stream << "\nRed-Black tree:\n";
	//visual_test(char_factory, stream);
	big_test(int_factory, stream);

#ifdef _WIN32
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}
