#pragma once
#include "json.h"
#include <string>
#include <vector>

namespace json {

class Builder;
class DictItemContext;
class ValueAfterKey;

class DictItemContext {
public:
	DictItemContext(Builder& b)
		:builder_(b)
	{}

	ValueAfterKey Key(std::string s);

	Builder& EndDict();


private:
	Builder& builder_;
};


class ArrayItemContext {
public:
	ArrayItemContext(Builder& b) 
		:builder_(b)
	{}

	ArrayItemContext Value(Node::Value value);

	DictItemContext StartDict();

	ArrayItemContext StartArray();

	Builder& EndArray();

private:
	Builder& builder_;
};


class ValueAfterKey {
public:
	ValueAfterKey(Builder& b)
		:builder_(b)
	{}

	DictItemContext Value(Node::Value value);

	DictItemContext StartDict();

	ArrayItemContext StartArray();

private:
	Builder& builder_;
};


class Builder {
public:
	Builder()
		:root_(Node(nullptr))
	{}

	Builder& Key(std::string s);

	Builder& Value(Node::Value value);

	DictItemContext StartDict();
	Builder& EndDict();

	ArrayItemContext StartArray();
	Builder& EndArray();

	const Node& Build();
private:

	Node root_;
	std::vector<Node*> nodes_stack_;

};



}