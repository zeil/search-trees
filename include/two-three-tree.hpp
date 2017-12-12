#pragma once

#include <utility>
#include <memory>

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
		NodePtr left, right, middle;

		Node(DataPtr &&data)
			: ldata(std::move(data))
		{}

		void print(std::ostream &stream)
		{
			stream << ldata->key;
			if (rdata) {
				stream << '|';
				stream << rdata->key;
			}
			stream << '\n';

			if (left)
				left->print(stream);
			if (middle)
				middle->print(stream);
			if (right)
				right->print(stream);
		}
	};

	NodePtr root;

	void insert_into_subtree(NodePtr &&subtree, NodePtr &&node)
	{
		if (!subtree)
			return;

		if (node->ldata->key <= subtree->ldata->key) {
			insert_into_subtree(std::move(subtree->left), std::move(node));
			if (!subtree->rdata) {
				subtree->rdata = std::move(subtree->ldata);
				subtree->ldata = std::move(node->ldata);
				subtree->left = std::move( node->left);
				subtree->middle = std::move(node->right);
				node.reset();
			} else {
			}
		} else if (subtree->rdata && node->ldata->key <= subtree->rdata->key) {
			insert_into_subtree(std::move(subtree->middle), std::move(node));
		} else {
			insert_into_subtree(std::move(subtree->right), std::move(node));
			if (!subtree->rdata) {
				subtree->rdata = std::move(node->ldata);
				subtree->middle = std::move(node->left);
				subtree->right = std::move(node->right);
				node.reset();
			} else {
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
			root->print(stream);
		else
			stream << "empty tree";
		stream << '\n';
	}
};
