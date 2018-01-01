#pragma once

#include <utility>
#include <string>

#include "search-tree.hpp"
#include "data.hpp"
#include "util.hpp"

namespace search_trees
{

template<typename Key, typename Value>
class TwoThreeTree final: public SearchTree<Key, Value>
{
	struct Node;
	using NodePtr = std::unique_ptr<Node>;

	struct Node
	{
		DataPtr<Key, Value> ldata, rdata;
		NodePtr left, middle, right;
		Node *parent;

		Node(DataPtr<Key, Value> &&data)
			: ldata(std::move(data))
			, parent(nullptr)
		{}

		bool is_three() const
		{
			return rdata != nullptr;
		}

		bool is_leaf() const
		{
			return !left;
		}

		void set_left(NodePtr &&node) {
			if (node)
				node->parent = this;
			left = std::move(node);
		}

		void set_middle(NodePtr &&node) {
			if (node)
				node->parent = this;
			middle = std::move(node);
		}

		void set_right(NodePtr &&node) {
			if (node)
				node->parent = this;
			right = std::move(node);
		}

		std::pair<Node *, bool> find(const Key &key)
		{
			if (key == ldata->key) {
				return std::make_pair(this, true);
			} else if (is_three() && key == rdata->key) {
				return std::make_pair(this, false);
			} else if (key < ldata->key) {
				if (left)
					return left->find(key);
				else
					return std::make_pair(nullptr, false);
			} else if (is_three() && key < rdata->key) {
				if (middle)
					return middle->find(key);
				else
					return std::make_pair(nullptr, false);
			} else {
				if (right)
					return right->find(key);
				else
					return std::make_pair(nullptr, false);
			}
		}

		Node *max()
		{
			if (!right)
				return this;

			auto r = right.get();
			for (; r->right; r = r->right.get());
			return r;
		}

		Node *min()
		{
			if (!left)
				return this;

			auto l = left.get();
			for (; l->left; l = l->left.get());
			return l;
		}

		Node *predecessor() const
		{
			if (!left)
				return nullptr;

			return left->max();
		}

		Node *successor() const
		{
			if (!right)
				return nullptr;

			return right->min();
		}

		friend std::ostream &operator<<(std::ostream &stream, const Node &node)
		{
			stream << node.ldata->key;
			if (node.rdata) {
				stream << '|';
				stream << node.rdata->key;
			}

			return stream;
		}

		void print(std::ostream &stream, const std::string &prefix, bool tail) const
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

			stream << prefix << (tail ? prefix1 : prefix2) << *this << '\n';

			if (right)
				right->print(stream, prefix + (tail ? prefix3 : prefix4), !middle && !left);
			if (middle)
				middle->print(stream, prefix + (tail ? prefix3 : prefix4), !left);
			if (left)
				left->print(stream, prefix + (tail ? prefix3 : prefix4), true);
		}
	};

	NodePtr root;

	static void insert_into_subtree(NodePtr &&subtree, NodePtr &&node)
	{
		if (!subtree)
			return;

		if (node->ldata->key == subtree->ldata->key) {
			subtree->ldata->value = std::move(node->ldata->value);
		} else if (subtree->is_three() && node->ldata->key == subtree->rdata->key) {
			subtree->rdata->value = std::move(node->ldata->value);
		} else if (node->ldata->key < subtree->ldata->key) {
			insert_into_subtree(std::move(subtree->left), std::move(node));
			if (!node)
				return;
			if (!subtree->is_three()) {
				subtree->rdata = std::move(subtree->ldata);
				subtree->ldata = std::move(node->ldata);
				subtree->set_left(std::move(node->left));
				subtree->set_middle(std::move(node->right));
				node.reset();
			} else {
				auto right = std::make_unique<Node>(std::move(subtree->rdata));
				right->set_left(std::move(subtree->middle));
				right->set_right(std::move(subtree->right));
				subtree->set_left(std::move(node));
				subtree->set_right(std::move(right));
				node = std::move(subtree);
			}
		} else if (subtree->is_three() && node->ldata->key < subtree->rdata->key) {
			insert_into_subtree(std::move(subtree->middle), std::move(node));
			if (!node)
				return;
			auto right = std::make_unique<Node>(std::move(subtree->rdata));
			right->set_left(std::move(node->right));
			right->set_right(std::move(subtree->right));
			subtree->set_right(std::move(node->left));
			node->parent = subtree->parent;
			node->set_left(std::move(subtree));
			node->set_right(std::move(right));
		} else {
			insert_into_subtree(std::move(subtree->right), std::move(node));
			if (!node)
				return;
			if (!subtree->is_three()) {
				subtree->rdata = std::move(node->ldata);
				subtree->set_middle(std::move(node->left));
				subtree->set_right(std::move(node->right));
				node.reset();
			} else {
				auto left = std::make_unique<Node>(std::move(subtree->ldata));
				left->set_left(std::move(subtree->left));
				left->set_right(std::move(subtree->middle));
				subtree->ldata = std::move(subtree->rdata);
				subtree->set_left(std::move(left));
				subtree->set_right(std::move(node));
				node = std::move(subtree);
			}
		}
	}

	template<typename KeyT, typename ValueT>
	void insert_impl(KeyT &&key, ValueT &&value)
	{
		auto data = std::make_unique<Data<Key, Value>>(std::forward<KeyT>(key), std::forward<ValueT>(value));

		if (root) {
			auto node = std::make_unique<Node>(std::move(data));
			insert_into_subtree(std::move(root), std::move(node));
			if (!root)
				root = std::move(node);
		} else {
			root = std::make_unique<Node>(std::move(data));
		}
	}

	void remove_hole(Node *hole)
	{
		auto parent = hole->parent;

		if (!parent) {
			root = std::move(hole->left);
			if (root)
				root->parent = nullptr;
			return;
		}

		if (!parent->is_three()) {
			if (hole == parent->left.get()) {
				auto sibling = parent->right.get();
				if (!sibling->is_three()) {
					sibling->rdata = std::move(sibling->ldata);
					sibling->ldata = std::move(parent->ldata);
					sibling->set_middle(std::move(sibling->left));
					sibling->set_left(std::move(hole->left));
					parent->set_left(std::move(parent->right));
					remove_hole(parent);
				} else {
					auto &left = parent->left;
					left->ldata = std::move(parent->ldata);
					left->set_right(std::move(sibling->left));
					parent->ldata = std::move(sibling->ldata);
					sibling->ldata = std::move(sibling->rdata);
					sibling->set_left(std::move(sibling->middle));
				}
			} else {
				auto sibling = parent->left.get();
				if (!sibling->is_three()) {
					sibling->rdata = std::move(parent->ldata);
					sibling->set_middle(std::move(sibling->right));
					sibling->set_right(std::move(hole->left));
					parent->right.reset();
					remove_hole(parent);
				} else {
					auto &right = parent->right;
					right->ldata = std::move(parent->ldata);
					right->set_right(std::move(right->left));
					right->set_left(std::move(sibling->right));
					parent->ldata = std::move(sibling->rdata);
					sibling->set_right(std::move(sibling->middle));
				}
			}
		} else {
			if (hole == parent->left.get()) {
				auto sibling = parent->middle.get();
				if (!sibling->is_three()) {
					sibling->rdata = std::move(sibling->ldata);
					sibling->ldata = std::move(parent->ldata);
					sibling->set_middle(std::move(sibling->left));
					sibling->set_left(std::move(hole->left));
					parent->ldata = std::move(parent->rdata);
					parent->set_left(std::move(parent->middle));
				} else {
					auto &left = parent->left;
					left->ldata = std::move(parent->ldata);
					left->set_right(std::move(sibling->left));
					parent->ldata = std::move(sibling->ldata);
					sibling->ldata = std::move(sibling->rdata);
					sibling->set_left(std::move(sibling->middle));
				}
			} else if (hole == parent->middle.get()) {
				auto sibling = parent->left.get();
				if (!sibling->is_three()) {
					sibling->rdata = std::move(parent->ldata);
					sibling->set_middle(std::move(sibling->right));
					sibling->set_right(std::move(hole->left));
					parent->ldata = std::move(parent->rdata);
					parent->middle.reset();
				} else {
					auto &middle = parent->middle;
					middle->ldata = std::move(parent->ldata);
					middle->set_right(std::move(middle->left));
					middle->set_left(std::move(sibling->right));
					parent->ldata = std::move(sibling->rdata);
					sibling->set_right(std::move(sibling->middle));
				}
			} else {
				auto sibling = parent->middle.get();
				if (!sibling->is_three()) {
					sibling->rdata = std::move(parent->rdata);
					sibling->set_middle(std::move(sibling->right));
					sibling->set_right(std::move(hole->left));
					parent->set_right(std::move(parent->middle));
				} else {
					auto &right = parent->right;
					right->ldata = std::move(parent->rdata);
					right->set_right(std::move(right->left));
					right->set_left(std::move(sibling->right));
					parent->rdata = std::move(sibling->rdata);
					sibling->set_right(std::move(sibling->middle));
				}
			}
		}
	}

	bool remove_impl(const Key &key)
	{
		if (root) {
			auto found = root->find(key);
			auto node = found.first;
			if (!node)
				return false;

			auto ldata = found.second;
			if (!node->is_leaf()) {
				if (ldata) {
					auto predecessor = node->predecessor();
					if (predecessor->is_three()) {
						node->ldata = std::move(predecessor->rdata);
					} else {
						node->ldata = std::move(predecessor->ldata);
						remove_hole(predecessor);
					}
				} else {
					auto successor = node->successor();
					if (successor->is_three()) {
						node->rdata = std::move(successor->ldata);
						successor->ldata = std::move(successor->rdata);
					} else {
						node->rdata = std::move(successor->ldata);
						remove_hole(successor);
					}
				}
			} else if (node->is_three()) {
				if (ldata)
					node->ldata = std::move(node->rdata);
				else
					node->rdata.reset();
			} else {
				node->ldata.reset();
				remove_hole(node);
			}

			return true;
		}

		return false;
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

	Value *find(const Key &key) override final
	{
		if (root) {
			auto found = root->find(key);
			auto node = found.first;
			if (node) {
				auto ldata = found.second;
				if (ldata)
					return &node->ldata->value;
				else
					return &node->rdata->value;
			}
		}

		return nullptr;
	}

	const Value *find(const Key &key) const override final
	{
		return find(key);
	}

	Value *min() override final
	{
		if (root)
			return &root->min()->ldata->value;

		return nullptr;
	}

	const Value *min() const override final
	{
		return min();
	}

	Value *max() override final
	{
		if (root) {
			auto max = root->max();
			if (max->is_three())
				return &max->rdata->value;
			else
				return &max->ldata->value;
		}

		return nullptr;
	}

	const Value *max() const override final
	{
		return max();
	}

	bool remove(const Key &key) override final
	{
		return remove_impl(key);
	}

	virtual void print(std::ostream &stream) override final
	{
		if (root)
			root->print(stream, "", true);
		else
			stream << "Empty tree";
		stream << '\n';
	}
};

} // namespace search_trees
