#include "json_builder.h"
#include <utility>
#include <memory>
#include <stdexcept>

namespace json {

using namespace std::literals;

Builder& Builder::Key(std::string s) {
	if (nodes_stack_.empty()) {
		throw std::logic_error("Node stack is empty"s);
	}
	if (!nodes_stack_.back()->IsDict()) {
		throw std::logic_error("Adding a key to a non-dictionary or before declaring the beginning of a dictionary"s);
	}
	auto& root_dict = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());
	auto& elem = std::get<Dict>(root_dict);
	elem[s];
	nodes_stack_.emplace_back(&elem.at(s));
	return *this;
}


Builder& Builder::Value(Node::Value value) {
	if (root_.IsNull()) {
		Node n;
		std::visit([&n](auto&& arg) {n = Node(std::move(arg)); }, value);
		root_ = std::move(n);
		return *this;
	}
	if (nodes_stack_.empty()) {
		throw std::logic_error("Node stack is empty"s);
	}
	if (nodes_stack_.back()->IsArray()) {
		auto& val = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());
		Array& arr = std::get<Array>(val);
		Node n;
		std::visit([&n](auto&& arg) {n = Node(std::move(arg)); }, value);
		arr.push_back(std::move(n));
		return *this;
	}
	if (!nodes_stack_.back()->IsNull()) {
		throw std::logic_error("The value is not the initial or the next after the key or an element of array"s);
	}

	auto& val = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());

	val.swap(value);
	nodes_stack_.pop_back();
	//Dict& root_dict = const_cast<Dict&>(nodes_stack_.back()->AsDict()); 

	return *this;
}

DictItemContext Builder::StartDict() {
	if (root_.IsNull()) {
		root_ = Node(Dict{});
		nodes_stack_.emplace_back(&root_);
		return DictItemContext(*this);
	}	
	if (nodes_stack_.empty()) {
		throw std::logic_error("The node stack is empty, but there are more operations"s);
	}
	if (nodes_stack_.back()->IsNull()) {
		auto& val = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());
		val = Dict{};
	}
	else if (nodes_stack_.back()->IsArray()) {
		auto& val = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());
		Array& arr = std::get<Array>(val);
		arr.push_back(Node(Dict{}));
		nodes_stack_.push_back(&arr.back());;
	}
	return DictItemContext(*this);
}

Builder& Builder::EndDict() {
	if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {
		nodes_stack_.pop_back();
		return *this;
	}
	throw std::logic_error("Calling the end of the dictionary without the beginning"s);
}

ArrayItemContext Builder::StartArray() {
	if (root_.IsNull()) {
		root_ = Node(Array{});
		nodes_stack_.emplace_back(&root_);
		return ArrayItemContext(*this);
	}
	if (nodes_stack_.empty()) {
		throw std::logic_error("The node stack is empty, but there are more operations"s);
	}
	if (nodes_stack_.back()->IsNull()) {
		auto& val = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());
		val = Array{};
	}
	else if (nodes_stack_.back()->IsArray()) {
		auto& val = const_cast<Node::Value&>(nodes_stack_.back()->GetValue());
		Array& arr = std::get<Array>(val);
		arr.push_back(Node(Array{}));
		nodes_stack_.push_back(&arr.back());
	}
	return ArrayItemContext(*this);
}

Builder& Builder::EndArray() {
	if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()) {
		nodes_stack_.pop_back();
		return *this;
	}
	throw std::logic_error("Calling the end of the array without the beginning"s);
}

const Node& Builder::Build() {
	if (!nodes_stack_.empty() || root_ == nullptr) {
		throw std::logic_error("The node stack is not empty during the build phase. Check that all *START sections are covered by *END sections"s);
	}
	return root_;
}

ValueAfterKey DictItemContext::Key(std::string s) {
	builder_.Key(s);
	return ValueAfterKey(builder_);
}

Builder& DictItemContext::EndDict() {
	builder_.EndDict();
	return builder_;
}

ArrayItemContext ArrayItemContext::Value(Node::Value value) {
	builder_.Value(value);
	return *this;
}

DictItemContext ArrayItemContext::StartDict() {
	builder_.StartDict();
	return DictItemContext(builder_);
}

ArrayItemContext ArrayItemContext::StartArray() {
	return builder_.StartArray();
}

Builder& ArrayItemContext::EndArray() {
	return builder_.EndArray();
}

DictItemContext ValueAfterKey::Value(Node::Value value) {
	builder_.Value(value);
	return DictItemContext(builder_);
}

DictItemContext ValueAfterKey::StartDict() {
	return builder_.StartDict();
}

ArrayItemContext ValueAfterKey::StartArray() {
	return builder_.StartArray();
}



}