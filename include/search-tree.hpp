#pragma once

#include <ostream>

template<typename Key, typename Value>
class SearchTree
{
public:
	virtual ~SearchTree() = default;

	virtual void insert(Key &&key, Value &&value) = 0;
	virtual bool remove(const Key &key) = 0;
	virtual Value *find(const Key &key) = 0;

	virtual void print(std::ostream &stream) = 0;
};
