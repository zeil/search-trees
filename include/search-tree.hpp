#pragma once

#include <memory>
#include <ostream>

namespace search_trees
{

template<typename Key, typename Value>
class SearchTree
{
public:
	virtual ~SearchTree() = default;

	virtual void insert(const Key &key, const Value &value) = 0;
	virtual void insert(const Key &key, Value &&value) = 0;
	virtual void insert(Key &&key, const Value &value) = 0;
	virtual void insert(Key &&key, Value &&value) = 0;

	virtual const Value *find(const Key &key) const = 0;
	virtual Value *find(const Key &key) = 0;

	virtual bool remove(const Key &key) = 0;

	virtual void print(std::ostream &stream) = 0;
};

template<typename Key, typename Value>
using SearchTreePtr = std::unique_ptr<SearchTree<Key, Value>>;

} // namespace search_trees
