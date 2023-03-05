#include "svg.h"

namespace svg {

    using namespace std::literals;



    std::ostream& operator<<(std::ostream& out, const StrokeLineCap& line) {
        switch (line)
        {

        case StrokeLineCap::BUTT:
            out << "butt"sv;
            break;

        case StrokeLineCap::ROUND:
            out << "round"sv;
            break;

        case StrokeLineCap::SQUARE:
            out << "square"sv;
            break;

        default:
            throw std::invalid_argument("Invalid Stroke");
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const StrokeLineJoin& line) {
        switch (line)
        {

        case StrokeLineJoin::ARCS:
            out << "arcs"sv;
            break;

        case StrokeLineJoin::BEVEL:
            out << "bevel"sv;
            break;

        case StrokeLineJoin::MITER:
            out << "miter"sv;
            break;

        case StrokeLineJoin::MITER_CLIP:
            out << "miter-clip"sv;
            break;

        case StrokeLineJoin::ROUND:
            out << "round"sv;
            break;

        default:
            throw;
        }
        return out;
    }

    std::ostream& operator<<(std::ostream& out, const Color& color) {
        std::visit(PathPropsPrinter{ out }, color);
        return out;
    }


    // ---------- Object ------------------

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // Делегируем вывод тега своим подклассам
        RenderObject(context);

        context.out << std::endl;
    }


    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
        out << "r=\""sv << radius_ << "\""sv;
        RenderAttrs(out);
        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        points_.push_back(point);
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<polyline points=\""sv;
        for (const auto& point : points_) {
            out << point.x << ","sv << point.y;
            if (&point != &points_[points_.size() - 1]) {
                out << " ";
            }
        }
        out << "\"";
        RenderAttrs(out);
        out << " />"sv;
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = std::move(pos);
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = std::move(offset);
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        font_size_ = size;
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "<text"sv;
        RenderAttrs(out);
        out << " x=\""sv << pos_.x << "\""sv;
        out << " y=\""sv << pos_.y << "\""sv;
        out << " dx=\""sv << offset_.x << "\""sv;
        out << " dy=\""sv << offset_.y << "\""sv;
        out << " font-size=\""sv << font_size_ << "\""sv;
        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }
        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }
        out << ">"sv;

        for (const char c : data_) {
            int num_char = static_cast<int>(c);
            if ((num_char >= 65 && num_char <= 90) || (num_char >= 97 && num_char <= 122)) {
                out << c;
                continue;
            }

            switch (c)
            {
            case '\"':
                out << "&quot;";
                break;
            case '\'':
                out << "&apos;";
                break;
            case '<':
                out << "&lt;";
                break;
            case '>':
                out << "&gt;";
                break;
            case '&':
                out << "&amp;";
                break;
            default:
                out << c;
            }
        }

        out << "</text>"sv;
    }

    // ---------- Document ------------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }


    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"sv << std::endl;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"sv << std::endl;
        RenderContext ctx(out, 2, 2);
        for (const std::unique_ptr<Object>& ptr : objects_) {
            ptr->Render(ctx);
        }
        out << "</svg>"sv;
    }

}  // namespace svg