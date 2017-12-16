#pragma once

#include <utility>
#include <memory>
#include <string>

#include "search-tree.hpp"
#include "util.hpp"

namespace search_trees
{

template<typename Key, typename Value>
class TwoThreeTree final: public SearchTree<Key, Value>
{
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

	using DataPtr = std::unique_ptr<Data>;

	struct Node;
	using NodePtr = std::unique_ptr<Node>;

	struct Node
	{
		DataPtr ldata, rdata;
		NodePtr left, middle, right;

		Node(DataPtr &&data)
			: ldata(std::move(data))
		{}

		bool is_three()
		{
			return rdata != nullptr;
		}

		void print(std::ostream &stream, const std::string &prefix, bool tail)
		{
		#ifdef _WIN32
			static const std::string prefix1 = { (char)192, (char)196, (char)196, (char) 32, 0 }; // "└── "
			static const std::string prefix2 = { (char)195, (char)196, (char)196, (char) 32, 0 }; // "├── "
			static const std::string prefix3 = { (char) 32, (char) 32, (char) 32, (char) 32, 0 }; // "    "
			static const std::string prefix4 = { (char)179, (char) 32, (char) 32, (char) 32, 0 }; // "│   "
		#else
			static const std::string prefix1 = "└── ";
			static const std::string prefix2 = "├── ";
			static const std::string prefix3 = "    ";
			static const std::string prefix4 = "│   ";
		#endif

			stream << prefix << (tail ? prefix1 : prefix2) << ldata->key;
			if (rdata) {
				stream << '|';
				stream << rdata->key;
			}
			stream << '\n';

			if (right)
				right->print(stream, prefix + (tail ? prefix3 : prefix4), !middle && !left);
			if (middle)
				middle->print(stream, prefix + (tail ? prefix3 : prefix4), !left);
			if (left)
				left->print(stream, prefix + (tail ? prefix3 : prefix4), true);
		}
	};

	NodePtr root;

	void insert_into_subtree(NodePtr &&subtree, NodePtr &&node)
	{
		if (!subtree)
			return;

		if (node->ldata->key <= subtree->ldata->key) {
			insert_into_subtree(std::move(subtree->left), std::move(node));
			if (!node)
				return;
			if (!subtree->is_three()) {
				subtree->rdata = std::move(subtree->ldata);
				subtree->ldata = std::move(node->ldata);
				subtree->left = std::move(node->left);
				subtree->middle = std::move(node->right);
				node.reset();
			} else {
				auto right = std::make_unique<Node>(std::move(subtree->rdata));
				right->left = std::move(subtree->middle);
				right->right = std::move(subtree->right);
				subtree->left = std::move(node);
				subtree->right = std::move(right);
				node = std::move(subtree);
			}
		} else if (node->is_three() && node->ldata->key <= subtree->rdata->key) {
			insert_into_subtree(std::move(subtree->middle), std::move(node));
			if (!node)
				return;
			auto right = std::make_unique<Node>(std::move(subtree->rdata));
			right->left = std::move(node->right);
			right->right = std::move(subtree->right);
			subtree->right = std::move(node->left);
			node->left = std::move(subtree);
			node->right = std::move(right);
		} else {
			insert_into_subtree(std::move(subtree->right), std::move(node));
			if (!node)
				return;
			if (!subtree->is_three()) {
				subtree->rdata = std::move(node->ldata);
				subtree->middle = std::move(node->left);
				subtree->right = std::move(node->right);
				node.reset();
			} else {
				auto left = std::make_unique<Node>(std::move(subtree->ldata));
				left->left = std::move(subtree->left);
				left->right = std::move(subtree->middle);
				subtree->ldata = std::move(subtree->rdata);
				subtree->left = std::move(left);
				subtree->right = std::move(node);
				node = std::move(subtree);
			}
		}
	}

	template<typename KeyT, typename ValueT>
	void insert_impl(KeyT &&key, ValueT &&value)
	{
		auto data = std::make_unique<Data>(std::forward<KeyT>(key), std::forward<ValueT>(value));

		if (root) {
			auto node = std::make_unique<Node>(std::move(data));
			insert_into_subtree(std::move(root), std::move(node));
			if (!root)
				root = std::move(node);
		} else {
			root = std::make_unique<Node>(std::move(data));
		}
	}

	std::pair<Node *, bool> find_node(const NodePtr &subtree, const Key &key) const
	{
		if (!subtree)
			return std::make_pair(nullptr, false);

		if (key == subtree->ldata->key) {
			return std::make_pair(subtree.get(), true);
		} else if (subtree->is_three() && key == subtree->rdata->key) {
			return std::make_pair(subtree.get(), false);
		} else if (key < subtree->ldata->key) {
			return find_node(subtree->left, key);
		} else if (subtree->is_three() && key < subtree->rdata->key) {
			return find_node(subtree->middle, key);
		} else {
			return find_node(subtree->right, key);
		}
	}

	Value *find_impl(const Key &key) const
	{
		auto found = find_node(root, key);
		auto node = found.first;
		if (node) {
			auto ldata = found.second;
			if (ldata)
				return &node->ldata->value;
			else
				return &node->rdata->value;
		}

		return nullptr;
	}

	TwoThreeTree() = default;

public:
	static SearchTreePtr<Key, Value> create()
	{
		return std::unique_ptr<TwoThreeTree<Key, Value>>(new TwoThreeTree<Key, Value>());
	}

	void insert(const Key &key, const Value &value) override final
	{
		insert_impl(key, value);
	}

	void insert(const Key &key, Value &&value) override final
	{
		insert_impl(key, value);
	}

	void insert(Key &&key, const Value &value) override final
	{
		insert_impl(key, value);
	}

	void insert(Key &&key, Value &&value) override final
	{
		insert_impl(key, value);
	}

	const Value *find(const Key &key) const override final
	{
		return find_impl(key);
	}

	Value *find(const Key &key) override final
	{
		return find_impl(key);
	}

	bool remove(const Key &key) override final
	{
		return false;
	}

	virtual void print(std::ostream &stream) override final
	{
		if (root)
			root->print(stream, "", true);
		else
			stream << "empty tree";
		stream << '\n';
	}
};

} // namespace search_trees
