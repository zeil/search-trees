#pragma once

#ifdef _WIN32
#include <Windows.h>
#endif

#include "search-tree.hpp"
#include "util.hpp"

namespace search_trees
{

template<typename Key, typename Value>
class RedBlackTree final: public SearchTree<Key, Value>
{
	struct Node;
	using NodePtr = std::unique_ptr<Node>;

	struct Node
	{
		DataPtr<Key, Value> data;
		NodePtr left, right;
		Node *parent;

		enum class Color {
			RED,
			BLACK
		} color;

		Node(DataPtr<Key, Value> &&data)
			: data(std::move(data))
			, parent(nullptr)
			, color(Color::RED)
		{}

		void set_left(NodePtr &&node) {
			if (node)
				node->parent = this;
			left = std::move(node);
		}

		void set_right(NodePtr &&node) {
			if (node)
				node->parent = this;
			right = std::move(node);
		}

		Node *find(const Key &key)
		{
			if (key == data->key) {
				return this;
			} else if (key < data->key) {
				if (left)
					return left->find(key);
				else
					return nullptr;
			} else {
				if (right)
					return right->find(key);
				else
					return nullptr;
			}
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

			stream << prefix << (tail ? prefix1 : prefix2);
			if (color == Color::RED) {
			#ifdef _WIN32
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 12);
				stream << data->key;
				SetConsoleTextAttribute(hConsole, 15);
			#else
				stream << "\033[31m" << data->key << "\033[0m";
			#endif
			} else {
				stream << data->key;
			}
			stream << '\n';

			if (right)
				right->print(stream, prefix + (tail ? prefix3 : prefix4), !left);
			if (left)
				left->print(stream, prefix + (tail ? prefix3 : prefix4), true);
		}
	};

	NodePtr root;

	void resolve_red_red_violation(Node *subtree)
	{
		if (!subtree)
			return;

		auto parent = subtree->parent;
		if (!parent)
			return;

		if (subtree == parent->left.get()) {
			if (subtree->left && subtree->left->color == Node::Color::RED) {
				auto left = std::move(subtree->left);
				auto right = std::move(parent->left);
				subtree->set_left(std::move(subtree->right));
				subtree->set_right(std::move(parent->right));
				parent->set_left(std::move(left));
				parent->set_right(std::move(right));
				parent->data.swap(parent->right->data);
			} else if (subtree->right && subtree->right->color == Node::Color::RED) {
				auto right = std::move(parent->right);
				parent->set_right(std::move(subtree->right));
				subtree->set_right(std::move(parent->right->left));
				parent->right->set_left(std::move(parent->right->right));
				parent->right->set_right(std::move(right));
				parent->data.swap(parent->right->data);
			} else {
				return;
			}
		} else if (subtree == parent->right.get()) {
			if (subtree->right && subtree->right->color == Node::Color::RED) {
				auto left = std::move(std::move(parent->right));
				auto right = std::move(subtree->right);
				subtree->set_left(std::move(parent->left));
				subtree->set_right(std::move(subtree->left));
				parent->set_left(std::move(left));
				parent->set_right(std::move(right));
				parent->data.swap(parent->left->data);
			} else if (subtree->left && subtree->left->color == Node::Color::RED) {
				auto left = std::move(parent->left);
				parent->set_left(std::move(subtree->left));
				subtree->set_left(std::move(parent->left->right));
				parent->left->set_right(std::move(parent->left->left));
				parent->left->set_left(std::move(left));
				parent->data.swap(parent->left->data);
			} else {
				return;
			}
		} else {
			return;
		}

		parent->left->color = Node::Color::BLACK;
		parent->right->color = Node::Color::BLACK;
		parent->color = Node::Color::RED;
		resolve_red_red_violation(parent->parent);
	}

	void insert_into_subtree(NodePtr &&subtree, NodePtr &&node)
	{
		if (!subtree)
			return;

		if (node->data->key <= subtree->data->key) {
			if (!subtree->left) {
				subtree->set_left(std::move(node));
				if (subtree->color == Node::Color::RED)
					resolve_red_red_violation(subtree.get());
			} else {
				insert_into_subtree(std::move(subtree->left), std::move(node));
			}
		} else {
			if (!subtree->right) {
				subtree->set_right(std::move(node));
				if (subtree->color == Node::Color::RED)
					resolve_red_red_violation(subtree.get());
			} else {
				insert_into_subtree(std::move(subtree->right), std::move(node));
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

		if (root->color != Node::Color::BLACK)
			root->color = Node::Color::BLACK;
	}

	Value *find_impl(const Key &key) const
	{
		if (root) {
			auto found = root->find(key);
			if (found)
				return &found->data->value;
		}

		return nullptr;
	}

	bool remove_impl(const Key &key)
	{
		return false;
	}

	RedBlackTree() = default;

public:
	static SearchTreePtr<Key, Value> create()
	{
		return std::unique_ptr<RedBlackTree<Key, Value>>(new RedBlackTree<Key, Value>());
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
		return remove_impl(key);
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
