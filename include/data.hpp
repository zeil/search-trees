#pragma once

#include <memory>

namespace search_trees
{

template<typename Key, typename Value>
struct Data
{
	Key key;
	Value value;

	Data(const Key &k, const Value &v)
		: key(k)
		, value(v)
	{}

	Data(const Key &k, Value &&v)
		: key(k)
		, value(std::move(v))
	{}

	Data(Key &&k, const Value &v)
		: key(std::move(k))
		, value(v)
	{}

	Data(Key &&k, Value &&v)
		: key(std::move(k))
		, value (std::move(v))
	{}
};

template<typename Key, typename Value>
using DataPtr = std::unique_ptr<Data<Key,Value>>;

} // namespace search_trees
