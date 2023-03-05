#include "map_renderer.h"

svg::Point renderer::SphereProjector::operator()(geo::Coordinates coords) const {
    return {
        (coords.lng - min_lon_) * zoom_coeff_ + padding_,
        (max_lat_ - coords.lat) * zoom_coeff_ + padding_
    };
}

bool renderer::IsZero(double value) {
    return std::abs(value) < EPSILON;
}

renderer::MapRenderer::MapRenderer(Settings& settings, SphereProjector& projector)
    :settings_(std::move(settings)), projector_(std::move(projector))
{}

void renderer::MapRenderer::AddText(const Bus* bus, svg::Document& doc) {

    svg::Text template_txt;

    template_txt.SetFontSize(settings_.bus_label_font_size)
        .SetOffset(settings_.bus_label_offset)
        .SetFontFamily("Verdana"s)
        .SetFontWeight("bold")
        .SetData(bus->bus_name);

    int last_route_end_stop = bus->stops_on_route.size() - 1;

    std::unique_ptr<svg::Text> bus_name = std::make_unique<svg::Text>();
    *bus_name = template_txt;
    bus_name->SetPosition(projector_(bus->stops_on_route[last_route_end_stop]->cooradinates));
    std::unique_ptr<svg::Text> bus_name_underlayer = std::make_unique<svg::Text>();
    *bus_name_underlayer = *bus_name;
    bus_name_underlayer->SetFillColor(settings_.underlayer_color)
        .SetStrokeColor(settings_.underlayer_color)
        .SetStrokeWidth(settings_.underlayer_width)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    bus_name->SetFillColor(settings_.color_palette[palette_color_num]);

    doc.AddPtr(std::move(bus_name_underlayer));
    doc.AddPtr(std::move(bus_name));

   // std::cout << "Num: " << bus->stops_on_route.size() / 2 << std::endl;

    //std::cout << "Last stop: " << bus->stops_on_route[last_route_end_stop]->stop_name << " " << std::endl;
    //std::cout << "Last route "s << bus->stops_on_route[bus->stops_on_route.size() / 2]->stop_name;
   

    if (int first_route_end_stop = bus->stops_on_route.size() / 2 ;  !bus->is_rounded && bus->stops_on_route[last_route_end_stop] != bus->stops_on_route[first_route_end_stop]) {
        std::unique_ptr<svg::Text> bus_visual_second = std::make_unique<svg::Text>();
        std::unique_ptr<svg::Text> bus_visual_second_underlayer = std::make_unique<svg::Text>();
        *bus_visual_second = template_txt;
        int returning_bus_number = bus->stops_on_route.size() / 2;
        bus_visual_second->SetPosition(projector_(bus->stops_on_route[returning_bus_number]->cooradinates));
        *bus_visual_second_underlayer = *bus_visual_second;
        bus_visual_second->SetFillColor(settings_.color_palette[palette_color_num]);
        bus_visual_second_underlayer->SetFillColor(settings_.underlayer_color)
            .SetStrokeColor(settings_.underlayer_color)
            .SetStrokeWidth(settings_.underlayer_width)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
        doc.AddPtr(std::move(bus_visual_second_underlayer));
        doc.AddPtr(std::move(bus_visual_second));
    }
    if (palette_color_num == static_cast<int>(settings_.color_palette.size() - 1)) {
        palette_color_num = 0;
    }
    else {
        ++palette_color_num;
    }
}

void renderer::MapRenderer::AddText(const Stop* stop, svg::Document& doc) {
    svg::Text template_txt;
    template_txt.SetPosition(projector_(stop->cooradinates))
        .SetOffset(settings_.stop_label_offset)
        .SetFontSize(settings_.stop_label_font_size)
        .SetFontFamily("Verdana"s)
        .SetData(stop->stop_name);

    std::unique_ptr<svg::Text> stop_label_underlayer = std::make_unique<svg::Text>();
    *stop_label_underlayer = template_txt;
    stop_label_underlayer->SetFillColor(settings_.underlayer_color)
        .SetStrokeColor(settings_.underlayer_color)
        .SetStrokeWidth(settings_.underlayer_width)
        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    std::unique_ptr<svg::Text> stop_label = std::make_unique < svg::Text>();
    *stop_label = template_txt;
    stop_label->SetFillColor("black"s);
    doc.AddPtr(std::move(stop_label_underlayer));
    doc.AddPtr(std::move(stop_label));
}

svg::Document renderer::MapRenderer::RenderObj(const TransportCatalogue& catalogue) {

    svg::Document doc;

    svg::Text stop_txt_template;
    stop_txt_template.SetFontSize(settings_.stop_label_font_size)
        .SetOffset(settings_.stop_label_offset);


    std::deque<const Bus*> sorted_buses;
    
    for (const Bus& bus : catalogue.GetAllBuses()) {
         sorted_buses.push_back(&bus);
    }

     std::sort(sorted_buses.begin(), sorted_buses.end(), [](const Bus* lhs, const Bus* rhs) {
        return lhs->bus_name < rhs->bus_name;
          }
     );
    
    for (const Bus* bus : sorted_buses) {

        std::unique_ptr<svg::Polyline> route = std::make_unique<svg::Polyline>();

        route->SetStrokeColor(settings_.color_palette[palette_color_num])
            .SetStrokeWidth(settings_.line_width)
            .SetFillColor(svg::NoneColor)
            .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
            .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

        for (const Stop* const stop : bus->stops_on_route) {
            const auto projected_stop = projector_(stop->cooradinates);
            route->AddPoint(projected_stop);      
        }

        if (palette_color_num == static_cast<int>(settings_.color_palette.size() - 1)) {
            palette_color_num = 0;
        }
        else {
            ++palette_color_num;
        }
        doc.AddPtr(std::move(route));

    }

    palette_color_num = 0;
    {
        auto cmp = [](const Stop* lhs, const Stop* rhs) {
            return std::lexicographical_compare(
                lhs->stop_name.begin(), lhs->stop_name.end(),
                rhs->stop_name.begin(), rhs->stop_name.end());
        };
        std::set<Stop*, decltype(cmp)> stops_set(cmp);

        for (const Bus* bus : sorted_buses) {
            AddText(bus, doc);
            stops_set.insert(bus->stops_on_route.begin(), bus->stops_on_route.end());
        }

        AddStopsOnRoute(stops_set, doc);
        for (const Stop* stop : stops_set) {
            AddText(stop, doc);
        }
    }

    return doc;
}
