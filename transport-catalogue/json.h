#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

    class Node;
    // Сохраните объявления Dict и Array без изменения
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:


        /* Реализуйте Node, используя std::variant */
        Node() = default;
        Node(std::nullptr_t);
        Node(Array array);
        Node(Dict dict);
        Node(int value);
        Node(double value);
        Node(std::string str);
        Node(bool value);

        bool IsNull() const;
        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsString() const;
        bool IsArray() const;
        bool IsMap() const;
        bool IsBool() const;

        double AsDouble() const;
        int AsInt() const;
        std::string AsString() const;
        bool AsBool() const;
        Array AsArray() const;
        Dict AsMap() const;

        Value GetValue() const;

    private:
        Value value_;
    };


    bool operator==(const Node& lhs, const Node& rhs);

    bool operator!=(const Node& lhs, const Node& rhs);

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    bool operator==(const Document& lhs, const Document& rhs);

    bool operator!=(const Document& lhs, const Document& rhs);

    struct PrintContext {
        std::ostream& out;
        int indent_step = 4;
        int indent = 0;

        void PrintIndent() const;
        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

    template<typename Value>
    void PrintValue(const Value& value, PrintContext& ctx);
    void PrintValue(std::nullptr_t, PrintContext& ctx);
    void PrintValue(const std::string& s, PrintContext& ctx);
    void PrintValue(const Array& array, PrintContext& ctx);
    void PrintValue(const Dict& dict, PrintContext& ctx);
    void PrintValue(const bool b, PrintContext& ctx);

    void PrintNode(const Node& node, PrintContext& ctx);

}  // namespace json

template<typename Value>
void json::PrintValue(const Value& value, PrintContext& ctx) {
    auto& out = ctx.out;
    out << value;
}