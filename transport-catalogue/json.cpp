#include "json.h"
#include <sstream>

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadArray(istream& input) {
            Array result;

            for (char c; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            if (input.eof()) {
                throw ParsingError("Error");
            }
            return Node(move(result));
        }

        using Number = std::variant<int, double>;

        Number LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            }
            else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    }
                    catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            }
            catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        std::string LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                }
                else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                    case 'n':
                        s.push_back('\n');
                        break;
                    case 't':
                        s.push_back('\t');
                        break;
                    case 'r':
                        s.push_back('\r');
                        break;
                    case '"':
                        s.push_back('"');
                        break;
                    case '\\':
                        s.push_back('\\');
                        break;
                    default:
                        // Встретили неизвестную escape-последовательность
                        throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                }
                else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                }
                else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return s;
        }

        Node LoadDict(istream& input) {
            Dict result;

            for (char c; input >> c && c != '}';) {
                if (c == ',' || c != '"') {
                    input >> c;
                }
                string key = LoadString(input);
                input >> c;
                result.insert({ move(key), LoadNode(input) });
            }
            if (input.eof()) {
                throw ParsingError("Error");
            }

            return Node(move(result));
        }

        bool LoadBool(istream& input) {
            std::string s;
            char c;
            for (; input >> c && c != ',' && (c >= 'a' && c <= 'z');) {
                s += c;
            }
            if (s == "true") {
                input.putback(c);
                return true;
            }
            if (s == "false") {
                input.putback(c);
                return false;
            }
            throw ParsingError("Error");
        }

        nullptr_t LoadNull(istream& input) {
            std::string s;
            for (char c; input >> c && c != ',';) {
                s += c;
            }
            if (s != "null") {
                throw ParsingError("Error");
            }
            return nullptr;
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;

            if (c == 'n') {
                input.putback(c);
                return LoadNull(input);
            }
            else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            }
            else if (c == '[') {
                return LoadArray(input);
            }
            else if (c == '{') {
                return LoadDict(input);
            }
            else if (c == '"') {
                return LoadString(input);
            }
            else {
                input.putback(c);
                auto var = LoadNumber(input);
                if (std::holds_alternative<int>(var)) {
                    return std::get<int>(var);
                }
                if (std::holds_alternative<double>(var)) {
                    return std::get<double>(var);
                }
                throw;
            }
        }

    }  // namespace

    Value Node::GetValue() const {
        return value_;
    }

    Node::Node(std::nullptr_t)
        :value_(nullptr)
    {}

    Node::Node(Array array)
        : value_(move(array)) {
    }

    Node::Node(Dict map)
        : value_(move(map)) {
    }

    Node::Node(int value)
        :value_(value)
    {}

    Node::Node(bool value)
        :value_(value)
    {}

    Node::Node(double value)
        :value_(value)
    {}

    Node::Node(std::string str)
        :value_(std::move(str))
    {}

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }

    bool Node::IsDouble() const {
        return IsPureDouble() || IsInt();
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }

    double Node::AsDouble() const {
        try {
            if (IsInt()) {
                return static_cast<double>(std::get<int>(value_));
            }
            return std::get<double>(value_);
        }
        catch (...) {
            throw std::logic_error("Can't reach this type");
        }
    }

    int Node::AsInt() const {
        try {
            return std::get<int>(value_);
        }
        catch (...) {
            throw std::logic_error("Can't reach this type");
        }
    }

    std::string Node::AsString() const {
        try {
            return std::get<std::string>(value_);
        }
        catch (...) {
            throw std::logic_error("Can't reach this type");
        }
    }

    bool Node::AsBool() const {
        try {
            return std::get<bool>(value_);
        }
        catch (...) {
            throw std::logic_error("Can't reach this type");
        }
    }

    Array Node::AsArray() const {
        try {
            return std::get<Array>(value_);
        }
        catch (...) {
            throw std::logic_error("Can't reach this type");
        }
    }

    Dict Node::AsMap() const {
        try {
            return std::get<Dict>(value_);
        }
        catch (...) {
            throw std::logic_error("Can't reach this type");
        }
    }

    bool operator==(const Node& lhs, const Node& rhs) {
        return lhs.GetValue() == rhs.GetValue();
    }

    bool operator!=(const Node& lhs, const Node& rhs) {
        return !(lhs == rhs);
    }

    Document::Document(Node root)
        : root_(move(root)) {
    }

    bool operator==(const Document& lhs, const Document& rhs) {
        return lhs.GetRoot() == rhs.GetRoot();
    }

    bool operator!=(const Document& lhs, const Document& rhs) {
        return !(lhs == rhs);
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    void PrintContext::PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    PrintContext PrintContext::Indented() const {
        return { out, indent_step, indent_step + indent };
    }

    Document Load(istream& input) {
        return Document{ LoadNode(input) };
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintContext ctx{ output };
        PrintNode(doc.GetRoot(), ctx);
    }

    void PrintValue(std::nullptr_t, PrintContext& ctx) {
        auto& out = ctx.out;
        out << "null"sv;
    }

    void PrintValue(const std::string& s, PrintContext& ctx) {
        auto& out = ctx.out;
        out << "\""sv;
        for (const char c : s) {

            switch (c)
            {
            case '\n':
                out << "\\n"sv;
                break;
            case '\r':
                out << "\\r"sv;
                break;
            case '"':
                out << "\\\""sv;
                break;
            case '\\':
                out << "\\\\"sv;
                break;

            default:
                out << c;
                break;
            }
        }
        out << "\""sv;

    }

    void PrintValue(const Array& array, PrintContext& ctx) {
        auto& out = ctx.out;
        out << '[';
        out << '\n';
        for (const Node& value : array) {
            auto ct = ctx.Indented();
            ct.PrintIndent();
            PrintNode(value, ct);
            if (&value != &array[array.size() - 1]) {
                out << ", "s;
            }
            out << '\n';

        }
        ctx.PrintIndent();
        out << ']';
    }

    void PrintValue(const Dict& dict, PrintContext& ctx) {
        auto& out = ctx.out;
        out << '{';
        out << '\n';
        for (const auto& elem : dict) {
            auto ct = ctx.Indented();
            ct.PrintIndent();
            PrintValue(elem.first, ct);
            out << ": ";
            PrintNode(elem.second, ct);
            if (&elem != &(*std::prev(dict.end(), 1))) {
                out << ',';
            }
            out << '\n';

        }
        ctx.PrintIndent();
        out << '}';
    }

    void PrintValue(const bool b, PrintContext& ctx) {
        auto& out = ctx.out;
        out << std::boolalpha << b;
    }

    void PrintNode(const Node& node, PrintContext& ctx) {
        std::visit(
            [&ctx](const auto& value) {PrintValue(value, ctx); },
            node.GetValue()
        );
    }

}  // namespace json