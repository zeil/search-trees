#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#include "two-three-tree.hpp"
#include "red-black-tree.hpp"

using namespace search_trees;

template<typename Key, typename Value>
class Add;

template<typename Key, typename Value>
class Delete;

template<typename Key, typename Value>
class Search;

template<typename Key, typename Value>
class Min;

template<typename Key, typename Value>
class Max;

template<typename Key, typename Value>
class Print;

template<typename Key, typename Value>
class Command
{
public:
	virtual ~Command() = default;

	virtual bool parse_args(const std::string &args) = 0;

	virtual void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) = 0;

	static std::unique_ptr<Command> parse(const std::string &line)
	{
		std::unique_ptr<Command> cmd;
		std::string args;

		if (strncmp(line.c_str(), "add", 3) == 0) {
			cmd = std::make_unique<Add<Key, Value>>();
			args = line.substr(3);
		} else if (strncmp(line.c_str(), "delete", 6) == 0) {
			cmd = std::make_unique<Delete<Key, Value>>();
			args = line.substr(6);
		} else if (strncmp(line.c_str(), "search", 6) == 0) {
			cmd = std::make_unique<Search<Key, Value>>();
			args = line.substr(6);
		} else if (strncmp(line.c_str(), "min", 3) == 0) {
			cmd = std::make_unique<Min<Key, Value>>();
			args = line.substr(3);
		} else if (strncmp(line.c_str(), "max", 3) == 0) {
			cmd = std::make_unique<Max<Key, Value>>();
			args = line.substr(3);
		} else if (strncmp(line.c_str(), "print", 5) == 0) {
			cmd = std::make_unique<Print<Key, Value>>();
			args = line.substr(5);
		}

		if (!cmd || !cmd->parse_args(args)) {
			std::cerr << "Unrecognized command: '" << line << "'\n";
			return nullptr;
		}

		return cmd;
	}
};

template<typename Key, typename Value>
class Add final: public Command<Key, Value>
{
public:
	Add() = default;

	bool parse_args(const std::string &args) override final
	{
		std::istringstream iss(args);

		return !!(iss >> key >> value);
	}

	void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) override final
	{
		tree->insert(key, value);
	}

private:
	Key key;
	Value value;
};

template<typename Key, typename Value>
class Delete final: public Command<Key, Value>
{
public:
	Delete() = default;

	bool parse_args(const std::string &args) override final
	{
		std::istringstream iss(args);

		return !!(iss >> key);
	}

	void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) override final
	{
		tree->remove(key);
	}

private:
	Key key;
};

template<typename Key, typename Value>
class Search final: public Command<Key, Value>
{
public:
	Search() = default;

	bool parse_args(const std::string &args) override final
	{
		std::istringstream iss(args);

		return !!(iss >> key);
	}

	void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) override final
	{
		auto value = tree->find(key);
		if (value)
			os << *value;
		else
			os << "Not found";
		os << '\n';
	}

private:
	Key key;
};

template<typename Key, typename Value>
class Min final: public Command<Key, Value>
{
public:
	Min() = default;

	bool parse_args(const std::string &args) override final
	{
		return true;
	}

	void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) override final
	{
		auto value = tree->min();
		if (value)
			os << *value;
		else
			os << "Not found";
		os << '\n';
	}
};

template<typename Key, typename Value>
class Max final: public Command<Key, Value>
{
public:
	Max() = default;

	bool parse_args(const std::string &args) override final
	{
		return true;
	}

	void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) override final
	{
		auto value = tree->max();
		if (value)
			os << *value;
		else
			os << "Not found";
		os << '\n';
	}
};

template<typename Key, typename Value>
class Print final: public Command<Key, Value>
{
public:
	Print() = default;

	bool parse_args(const std::string &args) override final
	{
		return true;
	}

	void exec(const SearchTreePtr<Key, Value> &tree, std::ostream &os) override final
	{
		tree->print(os);
	}
};

using KeyT = int;
using ValueT = int;

int main(int argc, char *argv[])
{
	const char *tree_type, *input_file, *output_file = nullptr;
	SearchTreePtr<KeyT, ValueT> tree;

	if (argc >= 3) {
		tree_type = argv[1];
		input_file = argv[2];
		if (argc >= 4)
			output_file = argv[3];
	} else {
		std::cerr << "Usage: " << argv[0] << " {rb,23} input.txt [output.txt]\n";
		return -1;
	}

	if (strcmp(tree_type, "rb") == 0) {
		tree = RedBlackTree<KeyT, ValueT>::create();
	} else if (strcmp(tree_type, "23") == 0) {
		tree = TwoThreeTree<KeyT, ValueT>::create();
	} else {
		std::cerr << "Invalid tree type '" << tree_type << "'. Available types: rb, 23\n";
		return -1;
	}

	std::ifstream ifs(input_file);
	if (!ifs) {
		std::cerr << "Failed to open file '" << input_file << "'\n";
		return -1;
	}

	std::ostream *os = &std::cout;
	std::ofstream ofs;
	if (output_file) {
		ofs.open(output_file);
		if (!ofs) {
			std::cerr << "Failed to open file '" << output_file << "'\n";
			return -1;
		}
		os = &ofs;
	}

	std::string line;
	while (std::getline(ifs, line)) {
		auto cmd = Command<KeyT, ValueT>::parse(line);
		cmd->exec(tree, *os);
	}

	return 0;
}
