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

		static void resolve_red_red_violation(Node *node)
		{
			if (!node || node->color == Node::Color::BLACK)
				return;

			auto parent = node->parent;
			if (!parent)
				return;

			if (node == parent->left.get()) {
				if (node->left && node->left->color == Node::Color::RED) {
					auto left = std::move(node->left);
					auto right = std::move(parent->left);
					node->set_left(std::move(node->right));
					node->set_right(std::move(parent->right));
					parent->set_left(std::move(left));
					parent->set_right(std::move(right));
					parent->data.swap(parent->right->data);
				} else if (node->right && node->right->color == Node::Color::RED) {
					auto right = std::move(parent->right);
					parent->set_right(std::move(node->right));
					node->set_right(std::move(parent->right->left));
					parent->right->set_left(std::move(parent->right->right));
					parent->right->set_right(std::move(right));
					parent->data.swap(parent->right->data);
				}
			} else if (node == parent->right.get()) {
				if (node->right && node->right->color == Node::Color::RED) {
					auto left = std::move(std::move(parent->right));
					auto right = std::move(node->right);
					node->set_right(std::move(node->left));
					node->set_left(std::move(parent->left));
					parent->set_left(std::move(left));
					parent->set_right(std::move(right));
					parent->data.swap(parent->left->data);
				} else if (node->left && node->left->color == Node::Color::RED) {
					auto left = std::move(parent->left);
					parent->set_left(std::move(node->left));
					node->set_left(std::move(parent->left->right));
					parent->left->set_right(std::move(parent->left->left));
					parent->left->set_left(std::move(left));
					parent->data.swap(parent->left->data);
				}
			}

			parent->left->color = Node::Color::BLACK;
			parent->right->color = Node::Color::BLACK;
			parent->color = Node::Color::RED;

			resolve_red_red_violation(parent->parent);
		}

		void insert(NodePtr &&node)
		{
			if (node->data->key == data->key) {
				data->value = std::move(node->data->value);
			} else if (node->data->key < data->key) {
				if (!left) {
					set_left(std::move(node));
					resolve_red_red_violation(this);
				} else {
					left->insert(std::move(node));
				}
			} else {
				if (!right) {
					set_right(std::move(node));
					resolve_red_red_violation(this);
				} else {
					right->insert(std::move(node));
				}
			}
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
			if (node.color == Color::RED) {
			#ifdef _WIN32
				HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				SetConsoleTextAttribute(hConsole, 12);
				stream << node.data->key;
				SetConsoleTextAttribute(hConsole, 15);
			#else
				stream << "\033[31m" << node.data->key << "\033[0m";
			#endif
			} else {
				stream << node.data->key;
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
				right->print(stream, prefix + (tail ? prefix3 : prefix4), !left);
			if (left)
				left->print(stream, prefix + (tail ? prefix3 : prefix4), true);
		}
	};

	NodePtr root;

	template<typename KeyT, typename ValueT>
	void insert_impl(KeyT &&key, ValueT &&value)
	{
		auto data = std::make_unique<Data<Key, Value>>(std::forward<KeyT>(key), std::forward<ValueT>(value));

		if (root) {
			auto node = std::make_unique<Node>(std::move(data));
			root->insert(std::move(node));
		} else {
			root = std::make_unique<Node>(std::move(data));
		}

		if (root->color != Node::Color::BLACK)
			root->color = Node::Color::BLACK;
	}

	static void remove_double_blackness(Node *node, Node *parent)
	{
		if (!parent)
			return;

		if (node && node->color == Node::Color::RED) {
			node->color = Node::Color::BLACK;
			return;
		}

		if (node == parent->left.get()) {
			auto sibling = parent->right.get();
			if (sibling) {
				if (sibling->color == Node::Color::BLACK) {
					if (sibling->left && sibling->left->color == Node::Color::RED) {
						auto left = std::move(sibling->left);
						left->color = Node::Color::BLACK;
						sibling->set_left(std::move(left->right));
						left->set_right(std::move(left->left));
						left->set_left(std::move(parent->left));
						parent->data.swap(left->data);
						parent->set_left(std::move(left));
					} else if (sibling->right && sibling->right->color == Node::Color::RED) {
						auto right = std::move(sibling->right);
						right->color = Node::Color::BLACK;
						sibling->set_right(std::move(sibling->left));
						sibling->set_left(std::move(parent->left));
						parent->data.swap(sibling->data);
						parent->set_left(std::move(parent->right));
						parent->set_right(std::move(right));
					} else if ((!sibling->left || (sibling->left && sibling->left->color == Node::Color::BLACK)) &&
							(!sibling->right || (sibling->right && sibling->right->color == Node::Color::BLACK))) {
						sibling->color = Node::Color::RED;
						remove_double_blackness(parent, parent->parent);
					}
				} else {
					auto left = std::move(parent->right);
					auto right = std::move(sibling->right);
					sibling->set_right(std::move(sibling->left));
					sibling->set_left(std::move(parent->left));
					parent->data.swap(left->data);
					parent->set_left(std::move(left));
					parent->set_right(std::move(right));
					remove_double_blackness(node, sibling);
				}
			}
		} else {
			auto sibling = parent->left.get();
			if (sibling) {
				if (sibling->color == Node::Color::BLACK) {
					if (sibling->right && sibling->right->color == Node::Color::RED) {
						auto right = std::move(sibling->right);
						right->color = Node::Color::BLACK;
						sibling->set_right(std::move(right->left));
						right->set_left(std::move(right->right));
						right->set_right(std::move(parent->right));
						parent->data.swap(right->data);
						parent->set_right(std::move(right));
					} else if (sibling->right && sibling->right->color == Node::Color::RED) {
						auto left = std::move(sibling->left);
						left->color = Node::Color::BLACK;
						sibling->set_left(std::move(sibling->right));
						sibling->set_right(std::move(parent->right));
						parent->data.swap(sibling->data);
						parent->set_right(std::move(parent->left));
						parent->set_left(std::move(left));
					} else if ((!sibling->left || (sibling->left && sibling->left->color == Node::Color::BLACK)) &&
							(!sibling->right || (sibling->right && sibling->right->color == Node::Color::BLACK))) {
						sibling->color = Node::Color::RED;
						remove_double_blackness(parent, parent->parent);
					}
				} else {
					auto right = std::move(parent->left);
					auto left = std::move(sibling->left);
					sibling->set_left(std::move(sibling->right));
					sibling->set_right(std::move(parent->right));
					parent->data.swap(right->data);
					parent->set_left(std::move(left));
					parent->set_right(std::move(right));
					remove_double_blackness(node, sibling);
				}
			}
		}
	}

	bool remove_impl(const Key &key)
	{
		if (root) {
			auto node = root->find(key);
			if (!node)
				return false;

			if (node->left) {
				auto predecessor = node->predecessor();
				node->data = std::move(predecessor->data);
				auto parent = predecessor->parent;
				auto child = predecessor->left.get();
				if (child) {
					if (node == parent)
						node->set_left(std::move(predecessor->left));
					else
						parent->set_right(std::move(predecessor->left));
				} else {
					if (predecessor == parent->left.get())
						parent->left.reset();
					else
						parent->right.reset();
				}
				remove_double_blackness(child, parent);
			} else if (node->right) {
				auto successor = node->successor();
				node->data = std::move(successor->data);
				auto parent = successor->parent;
				auto child = successor->right.get();
				if (child) {
					if (node == parent)
						node->set_right(std::move(successor->right));
					else
						parent->set_left(std::move(successor->right));
				} else {
					if (successor == parent->left.get())
						parent->left.reset();
					else
						parent->right.reset();
				}
				remove_double_blackness(child, parent);
			} else {
				auto parent = node->parent;
				if (parent) {
					if (node == parent->left.get())
						parent->left.reset();
					else
						parent->right.reset();
					remove_double_blackness(nullptr, parent);
				} else {
					root.reset();
				}
			}

			return true;
		}

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

	Value *find(const Key &key) override final
	{
		if (root) {
			auto node = root->find(key);
			if (node)
				return &node->data->value;
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
			return &root->min()->data->value;

		return nullptr;
	}

	const Value *min() const override final
	{
		return min();
	}

	Value *max() override final
	{
		if (root)
			return &root->max()->data->value;

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
