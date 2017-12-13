#pragma once

#include <utility>
#include <memory>
#include <string>

#include "search-tree.hpp"

template<typename Key, typename Value>
class TwoThreeTree final: public SearchTree<Key, Value>
{
	struct Data
	{
		Key key;
		Value value;

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

		void print(std::ostream &stream, const std::string &prefix, bool tail)
		{
			static const std::string prefix1 = { (char)192, (char)196, (char)196, (char) 32, 0 }; // "└── "
			static const std::string prefix2 = { (char)195, (char)196, (char)196, (char) 32, 0 }; // "├── "
			static const std::string prefix3 = { (char) 32, (char) 32, (char) 32, (char) 32, 0 }; // "    "
			static const std::string prefix4 = { (char)179, (char) 32, (char) 32, (char) 32, 0 }; // "│   "

			stream << prefix << (tail ? prefix1 : prefix2) << ldata->key;
			if (rdata) {
				stream << '|';
				stream << rdata->key;
			}
			stream << '\n';

			if (left)
				left->print(stream, prefix + (tail ? prefix3 : prefix4), !middle && !right);
			if (middle)
				middle->print(stream, prefix + (tail ? prefix3 : prefix4), !right);
			if (right)
				right->print(stream, prefix + (tail ? prefix3 : prefix4), true);
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
			if (!subtree->rdata) {
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
		} else if (subtree->rdata && node->ldata->key <= subtree->rdata->key) {
			insert_into_subtree(std::move(subtree->middle), std::move(node));
			if (!node)
				return;
			auto right = std::make_unique<Node>(std::move(subtree->rdata));
			right->left = std::move(node->right);
			right->right = std::move(subtree->right);
			subtree->ldata.swap(node->ldata);
			node->right = std::move(node->left);
			node->left = std::move(subtree->left);
			subtree->left = std::move(node);
			subtree->right = std::move(right);
			node = std::move(subtree);
		} else {
			insert_into_subtree(std::move(subtree->right), std::move(node));
			if (!node)
				return;
			if (!subtree->rdata) {
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

public:
	void insert(Key &&key, Value &&value) override final
	{
		auto data = std::make_unique<Data>(std::move(key), std::move(value));

		if (root) {
			auto node = std::make_unique<Node>(std::move(data));
			insert_into_subtree(std::move(root), std::move(node));
			if (!root)
				root = std::move(node);
		} else {
			root = std::make_unique<Node>(std::move(data));
		}
	}

	bool remove(const Key &key) override final
	{
		return false;
	}

	Value *find(const Key &key) override final
	{
		return nullptr;
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
