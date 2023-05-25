#include "serialization.h"

void print(serial::TransportCatalogue& c) {
    const auto& a = c.stop_storage();
    std::cerr << "Stop_Storage: "s << std::endl;
    for (const auto& stop : a.stop_storage()) {
        std::cerr << "Stopname: "s << stop.name() << std::endl;
    }

    const auto b = c.bus_storage();

    std::cerr << "---------------"s << std::endl;
    std::cerr << "Bus_storage: "s << std::endl;
    for (const auto& bus : b.bus_storage()) {
        std::cerr << "Busname: "s << bus.name() << std::endl;
        std::cerr << "Amount of stops on the route: "s << bus.stops_id_size() << std::endl;
    }

    const auto f = c.stops_to_dist();

    std::cerr << "---------------"s << std::endl;

    for (const auto& elem : f) {
        std::cerr << "Stop_pair: {"s << elem.stops_between().first_stop() << " | " << elem.stops_between().last_stop() << "}, Dist: "s << elem.dist() << std::endl;
    }

    std::cerr << "---------------"s << std::endl;


    {
        const auto l = c.render_settings();
        std::cerr << "Render settings: "s << std::endl;
        std::cerr << "    height: "s << l.height() << '\n';
        std::cerr << "    width: "s << l.width() << '\n';
        std::cerr << "    padding: "s << l.padding() << '\n';
        std::cerr << "    libe_width: "s << l.line_width() << '\n';
        std::cerr << "    stop_radius: "s << l.stop_radius() << '\n';
        std::cerr << "    bus_label_font_size: "s << l.bus_label_font_size() << '\n';
        std::cerr << "    bus_label_offset: x="s << l.bus_label_offset().x() << " y="s << l.bus_label_offset().y() << '\n';
        std::cerr << "    stop_label_font_size: "s << l.stop_label_font_size() << '\n';
        std::cerr << "    stop_label_offset: x="s << l.stop_label_offset().x() << " y="s << l.stop_label_offset().y() << '\n';
        if (l.underlayer_color().has_solid_color()) {
            std::cerr << "    underlayer_color: "s << l.underlayer_color().solid_color().color() << '\n';
        }else if (l.underlayer_color().has_rgb()) {
            const auto& a = l.underlayer_color().rgb();
            std::cerr << "    underlayer_color: RGB("s << a.r() << ' ' << a.g() << ' ' << a.b() << ")"s << "\n";
        }else if (l.underlayer_color().has_rgba()) {
            const auto& a = l.underlayer_color().rgba().rgb();
            std::cerr << "    underlayer_color: RGBA("s << a.r() << ' ' << a.g() << ' ' << a.b() << l.underlayer_color().rgba().a() << ")"s << "\n";
        }else {
            std::cerr << "    underlayer_color: MONOSTATE"s << '\n';
        }

        std::cerr << "    underlayer_width: "s << l.underlayer_width() << '\n';

        const auto& palette = l.color_palette();
        for (const auto& color : palette) {
            if (l.underlayer_color().has_solid_color()) {
                std::cerr << "    One of the colors in the palette: "s << l.underlayer_color().solid_color().color() << '\n';
            }else if (l.underlayer_color().has_rgb()) {
                const auto& a = l.underlayer_color().rgb();
                std::cerr << "    One of the colors in the palette: RGB("s << a.r() << ' ' << a.g() << ' ' << a.b() << ")"s << "\n";
            }else if (l.underlayer_color().has_rgba()) {
                const auto& a = l.underlayer_color().rgba().rgb();
                std::cerr << "    One of the colors in the palette: RGBA("s << a.r() << ' ' << a.g() << ' ' << a.b() << ' ' << l.underlayer_color().rgba().a() << ")"s << "\n";
            }else {
                std::cerr << "    One of the colors in the palette: MONOSTATE"s << '\n';
            }
        }

    }

}


void TransportSerializer::Serialize(std::ostream& out) {
    ParseStop();
    ParseBus();
    ParseStopToDist();
    ParseRenderSettings();
    ParseRoutingSettings();
    ParseTR();


    //print(serialize_file);

    serialize_file.SerializeToOstream(&out);
}

void TransportSerializer::ParseStop() {
    int id = 0;
    for (const Stop& stop : catalogue_ref_.GetAllStops()) {
        serial::StopList::Stop stop_serial = StopToSerialStop(stop, id);
        id_to_stopname_[id] = stop.stop_name;
        stopname_to_id[stop.stop_name] = id;
        serialize_file.mutable_stop_storage()->mutable_stop_storage()->Add(std::move(stop_serial));
        ++id;
    }
}

void TransportSerializer::ParseBus() {
    int id = 0;
    for (const Bus& bus : catalogue_ref_.GetAllBuses()) {
        serial::BusList::Bus bus_serial = BusToSerialBus(bus, id);
        serialize_file.mutable_bus_storage()->mutable_bus_storage()->Add(std::move(bus_serial));
        busname_to_id_[bus.bus_name] = id;
        ++id;
    }
}

void TransportSerializer::ParseStopToDist() {
    const auto& stops_to_dist_cont = catalogue_ref_.GetStopsToDist();
    for (const auto& [stop_pair, dist] : stops_to_dist_cont) {
        serialize_file.mutable_stops_to_dist()->Add(StopsDistToSerial(stop_pair, dist));
    }

}


serial::BusList::Bus TransportSerializer::BusToSerialBus(const Bus& bus, int id) {
    serial::BusList::Bus res;
    res.set_name(bus.bus_name);
    res.set_is_rounded(bus.is_rounded);
    res.set_id(id);
    for (const Stop* stop : bus.stops_on_route) {
        res.mutable_stops_id()->Add(stopname_to_id.at(stop->stop_name));
    }
    return res;
}

serial::StopList::Stop TransportSerializer::StopToSerialStop(const Stop& stop, int id) {
    serial::StopList::Stop res;
    res.set_name(stop.stop_name);
    res.set_lat(stop.cooradinates.lat);
    res.set_lng(stop.cooradinates.lng);
    res.set_id(id);
    return res;
}


serial::StopsToDist TransportSerializer::StopsDistToSerial(const std::pair<const Stop*, const Stop*>& stop_pair, double dist) {
    serial::StopsToDist res;

    serial::StopsToDist::StopIdPair stops_between;
    stops_between.set_first_stop(stopname_to_id.at(stop_pair.first->stop_name));
    stops_between.set_last_stop(stopname_to_id.at(stop_pair.second->stop_name));
    *res.mutable_stops_between() = std::move(stops_between);
    res.set_dist(dist);
    return res;
}


void TransportSerializer::ParseRenderSettings() {

    serial::RenderSettings serial_render_settings;
    //const Dict& render_settings = json_doc_.GetRoot().AsDict().at("render_settings"s).AsDict();
    renderer::Settings render_settings = map_ref_.GetSettings();

    serial_render_settings.set_width(render_settings.width);
    serial_render_settings.set_height(render_settings.height);
    serial_render_settings.set_padding(render_settings.padding);
    serial_render_settings.set_line_width(render_settings.line_width);
    serial_render_settings.set_stop_radius(render_settings.stop_radius);
    serial_render_settings.set_stop_label_font_size(render_settings.stop_label_font_size);

    serial::Offset b_off;
    b_off.set_x(render_settings.bus_label_offset.x);
    b_off.set_y(render_settings.bus_label_offset.y);
    *serial_render_settings.mutable_bus_label_offset() = std::move(b_off);

    serial_render_settings.set_bus_label_font_size(render_settings.bus_label_font_size);

    serial::Offset s_off;
    s_off.set_x(render_settings.stop_label_offset.x);
    s_off.set_y(render_settings.stop_label_offset.y);
    *serial_render_settings.mutable_stop_label_offset() = std::move(s_off);

    *serial_render_settings.mutable_underlayer_color() = ParseColor(render_settings.underlayer_color);

    serial_render_settings.set_underlayer_width(render_settings.underlayer_width);

    for (const auto& color : render_settings.color_palette) {
        serial_render_settings.mutable_color_palette()->Add(ParseColor(color));
    }

    *serialize_file.mutable_render_settings() = std::move(serial_render_settings);

}

serial::Color TransportSerializer::ParseColor(const svg::Color& color) {
    serial::Color color_serial;
    if (std::holds_alternative<std::string>(color)) {
        serial::SolidColor solid_c;
        solid_c.set_color(std::get<std::string>(color));
        *color_serial.mutable_solid_color() = std::move(solid_c);
    }
    else if (std::holds_alternative<svg::Rgba>(color)) {
        const svg::Rgba& rgba = std::get<svg::Rgba>(color);
        serial::RGBA rgba_c;
        rgba_c.mutable_rgb()->set_r(rgba.red);
        rgba_c.mutable_rgb()->set_g(rgba.green);
        rgba_c.mutable_rgb()->set_b(rgba.blue);
        rgba_c.set_a(rgba.opacity);
        *color_serial.mutable_rgba() = std::move(rgba_c);
    }
    else if (std::holds_alternative<svg::Rgb>(color)) {
        const svg::Rgb& rgb = std::get<svg::Rgb>(color);
        serial::RGB rgb_c;
        rgb_c.set_r(rgb.red);
        rgb_c.set_g(rgb.green);
        rgb_c.set_b(rgb.blue);
        *color_serial.mutable_rgb() = std::move(rgb_c);
    }

    return color_serial;
}


void TransportSerializer::ParseTR() {

    serial::RoutingSettings routing_settings = ParseRoutingSettings();

    serial::Graph serial_graph = ParseGraph();
    serial::Router serial_router = ParseRouter();
    *serial_router.mutable_graph() = std::move(serial_graph);

    *serialize_file.mutable_transport_router()->mutable_router() = std::move(serial_router);
    *serialize_file.mutable_transport_router()->mutable_routing_settings() = std::move(routing_settings);
}

serial::Graph TransportSerializer::ParseGraph() {
    const auto& graph = router_.GetGraph();
    serial::Graph serial_graph;
    for (int i = 0; i < graph.GetEdgeCount(); ++i) {
        serial::Edge edge_serial;
        InsertEdgesInfo(i); //заполняем мапы TransportRouter'а
        const auto& edge_normal = graph.GetEdge(i);
        edge_serial.set_from(edge_normal.from);
        edge_serial.set_to(edge_normal.to);
        edge_serial.set_is_waiting(edge_normal.is_waiting);
        edge_serial.set_weight(edge_normal.weight);
        serial_graph.mutable_edges()->Add(std::move(edge_serial));
    }

    serial_graph.set_vertex_amount(graph.GetVertexCount());
    return serial_graph;
}

void TransportSerializer::InsertEdgesInfo(graph::EdgeId edge_id) {
    const auto& edge_info = router_.GetEdgeInfoByEdgeId(edge_id);
    auto* router_serial = serialize_file.mutable_transport_router();
    serial::EdgeInfo serial_info;
    if (std::holds_alternative<WaitEdge>(edge_info)) {
        const auto& wait = std::get<WaitEdge>(edge_info);
        //std::string_view stop_name = catalogue_ref_.GetStop(wait.stop_name).stop_name;
        const int stop_id = stopname_to_id.at(wait.stop_name);
        const auto& vertex_info = router_.GetStopVertexInfo(wait.stop_name);
        serial_info.mutable_w_e()->set_stop_id(stop_id);
        serial_info.mutable_w_e()->set_time(wait.time);
        router_serial->mutable_edge_info_by_edge_id()->insert({edge_id, std::move(serial_info)});

        serial::StopGraphContain vertex_info_serial;
        vertex_info_serial.set_start_waiting(vertex_info.start_waiting);
        vertex_info_serial.set_end_waiting(vertex_info.end_waiting);
        router_serial->mutable_stops_vertex_cont()->insert({stop_id, std::move(vertex_info_serial)});

    }else if (std::holds_alternative<BusEdge>(edge_info)) {
        const auto& bus_edge = std::get<BusEdge>(edge_info);
        const int bus_id = busname_to_id_.at(bus_edge.bus_name);
        serial_info.mutable_b_e()->set_bus_id(bus_id);
        serial_info.mutable_b_e()->set_span_count(bus_edge.span_count);
        serial_info.mutable_b_e()->set_time(bus_edge.time);
        router_serial->mutable_edge_info_by_edge_id()->insert({edge_id, std::move(serial_info)});
    }
}

serial::Router TransportSerializer::ParseRouter() {
    const auto& data = router_.GetRouter().GetRoutesIternalData();
    serial::Router serial_router;
    for (int i = 0; i < data.size(); ++i) {
        serial_router.add_routes_internal_data();
        for (int k = 0; k < data[i].size(); ++k) {
            serial_router.mutable_routes_internal_data()->at(i).add_value();
        }
    }

    auto& route_iternal_data = *serial_router.mutable_routes_internal_data();

    for (int i = 0; i < data.size(); ++i) {
        const auto& first_layer = data[i];
        for (int k = 0; k < first_layer.size(); ++k) {
            serial::RouteInternalData d;
            if (first_layer[k].has_value()) {
                if (first_layer[k]->prev_edge.has_value()) {
                    d.mutable_prev_edge_id()->set_value(*first_layer[k]->prev_edge);
                }
                d.set_weight_val(first_layer[k]->weight);
                d.set_is_initialized(true);
                route_iternal_data.at(i).mutable_value()->at(k) = std::move(d);
            }
        }
    }
    return serial_router;
}

serial::RoutingSettings TransportSerializer::ParseRoutingSettings() {
    serial::RoutingSettings routing_serial;

    routing_serial.set_bus_velocity(routing_settings_.speed);
    routing_serial.set_bus_wait_time(routing_settings_.wait_time);

    return std::move(routing_serial);
}

//---------------Deserialize-----------------------------------------------

TransportCatalogue TransportDeserializer::DeserializeCatalogue() {
    return ConvertTC();
}

renderer::MapRenderer TransportDeserializer::DeserializeMap(const TransportCatalogue& c) {
    return ConvertMap(c, ConvertMapSettings());
}

TransportRouter TransportDeserializer::DeserializeRouter(const TransportCatalogue& c) {
    return ConvertTransportRouter(c);
}


TransportCatalogue TransportDeserializer::ConvertTC() {

    //std::cerr << "==========="s << std::endl;


    TransportCatalogue catalogue;
    //print(serial_);

    //Добавление остановок в stop_storage
    for (const auto& stop : serial_.stop_storage().stop_storage()) {
        Stop* stop_ptr = catalogue.AddStop(SerialStopToStop(stop));
        id_to_stopname_[stop.id()] = stop_ptr->stop_name;
        stopname_to_id_[stop_ptr->stop_name] = stop.id();
    }

    //Добавление расстояний между остановками

    for (const serial::StopsToDist& elem : serial_.stops_to_dist()) {
        auto a = NormalizeSerialStopToDist(elem, catalogue);
        catalogue.AddDistance(a.first, a.second);
    }

    //Добавление автобусов(маршрутов)
    for (const auto& bus : serial_.bus_storage().bus_storage()) {
        const Bus* bus_ptr = catalogue.AddBus(SerialBusToBus(bus, catalogue));
        catalogue.AddRouteLength(bus_ptr->bus_name, catalogue.ComputeRouteLength(*bus_ptr));
        id_to_busname_[bus.id()] = bus_ptr->bus_name;

    }
    return catalogue;
}

renderer::MapRenderer TransportDeserializer::ConvertMap(const TransportCatalogue& catalogue, const renderer::Settings& map_settings) const  {
    std::deque<Stop> correct_stops = MakeOnlyUsedStops(catalogue);
    renderer::SphereProjector projector(
        correct_stops.begin(),
        correct_stops.end(),
        map_settings.width,
        map_settings.height,
        map_settings.padding
        );
    renderer::MapRenderer map(map_settings, std::move(projector));
    return map;
}

renderer::Settings TransportDeserializer::ConvertMapSettings() const {
    renderer::Settings map_settings;


    serial::RenderSettings serial_settings = serial_.render_settings();
    map_settings.width = serial_settings.width();
    map_settings.height = serial_settings.height();
    map_settings.padding = serial_settings.padding();
    map_settings.line_width = serial_settings.line_width();
    map_settings.stop_radius = serial_settings.stop_radius();
    map_settings.bus_label_font_size = serial_settings.bus_label_font_size();
    map_settings.bus_label_offset.x = serial_settings.bus_label_offset().x();
    map_settings.bus_label_offset.y = serial_settings.bus_label_offset().y();
    map_settings.stop_label_font_size = serial_settings.stop_label_font_size();
    map_settings.stop_label_offset.x = serial_settings.stop_label_offset().x();
    map_settings.stop_label_offset.y = serial_settings.stop_label_offset().y();
    map_settings.underlayer_width = serial_settings.underlayer_width();

    const auto& under_color_serial = serial_settings.underlayer_color();
    map_settings.underlayer_color = ColorCheck(under_color_serial);

    for (const serial::Color& color_serial : serial_settings.color_palette()) {
        map_settings.color_palette.push_back(ColorCheck(color_serial));
    }

    return map_settings;
}


TransportRouter TransportDeserializer::ConvertTransportRouter(const TransportCatalogue& catalogue) const {

    TransportRouter tr(
        ConvertRoutingSettings(), catalogue,
        ConvertGraph(), ConvertIternalData(),
        ConvertHashEdgeInfoToId(catalogue), ConvertHashStopVertexesCont(catalogue));

    return tr;
}

std::unordered_map<graph::EdgeId, EdgeInfo> TransportDeserializer::ConvertHashEdgeInfoToId(const TransportCatalogue& catalogue) const {
    const auto& edge_info_by_id = serial_.transport_router().edge_info_by_edge_id();
    std::unordered_map<graph::EdgeId, EdgeInfo> edge_info_by_id_hash;
    for (const auto& [edge_id, edge_info] : edge_info_by_id) {
        EdgeInfo temp;
        if (edge_info.has_b_e()) {
            BusEdge bus_edge_norm;
            bus_edge_norm.bus_name = id_to_busname_.at(edge_info.b_e().bus_id());
            bus_edge_norm.span_count = edge_info.b_e().span_count();
            bus_edge_norm.time = edge_info.b_e().time();
            temp = std::move(bus_edge_norm);
        }else if (edge_info.has_w_e()) {
            WaitEdge wait_edge_norm;
            wait_edge_norm.stop_name = id_to_stopname_.at(edge_info.w_e().stop_id());
            wait_edge_norm.time = edge_info.w_e().time();
            temp = std::move(wait_edge_norm);
        }
        edge_info_by_id_hash[edge_id] = std::move(temp);
    }
    return edge_info_by_id_hash;
}

std::unordered_map<std::string_view, StopGraphContain> TransportDeserializer::ConvertHashStopVertexesCont(const TransportCatalogue& catalogue) const {
    const auto& stops_vertex_cont = serial_.transport_router().stops_vertex_cont();
    std::unordered_map<std::string_view, StopGraphContain> stops_vertex_cont_hash;

    for (const auto& [stop_id, stop_vert_cont] : stops_vertex_cont) {
        const Stop& stop_ref = catalogue.GetStop(id_to_stopname_.at(stop_id));
        StopGraphContain stop_vert = {stop_ref, stop_vert_cont.start_waiting(), stop_vert_cont.end_waiting()};
        stops_vertex_cont_hash.insert({stop_ref.stop_name, std::move(stop_vert)});
    }
    return stops_vertex_cont_hash;
}

graph::DirectedWeightedGraph<double> TransportDeserializer::ConvertGraph() const {

    const auto& serial_graph = serial_.transport_router().router().graph();
    graph::DirectedWeightedGraph<double> graph_normal(serial_graph.vertex_amount());

    for (const serial::Edge& serial_edge : serial_graph.edges()) {
        graph::Edge<double> normal_edge;
        normal_edge.from = serial_edge.from();
        normal_edge.to = serial_edge.to();
        normal_edge.is_waiting = serial_edge.is_waiting();
        normal_edge.weight = serial_edge.weight();
        graph_normal.AddEdge(std::move(normal_edge));
    }
    return graph_normal;
}

graph::Router<double>::RoutesInternalData TransportDeserializer::ConvertIternalData() const {
    const serial::Router router_serial = serial_.transport_router().router();
    graph::Router<double>::RoutesInternalData data(
        router_serial.routes_internal_data_size(),
        std::vector<std::optional<graph::Router<double>::RouteInternalData>>(router_serial.routes_internal_data_size())
        );

    for (int i = 0; i < router_serial.routes_internal_data_size(); ++i) {
        for (int k = 0; k < router_serial.routes_internal_data().at(i).value_size(); ++k) {
            if (router_serial.routes_internal_data(i).value(k).is_initialized()) {
                graph::Router<double>::RouteInternalData temp;
                const auto& elem = router_serial.routes_internal_data(i).value(k);
                temp.weight = elem.weight_val();
                if (elem.has_prev_edge_id()) {
                    temp.prev_edge = elem.prev_edge_id().value();
                }
                data[i][k] = std::move(temp);
            }
        }
    }
    return data;
}

svg::Color TransportDeserializer::ColorCheck(serial::Color serial_color) const {
    if (serial_color.has_solid_color()) {
        return serial_color.solid_color().color();
    }else if (serial_color.has_rgb()) {
        const auto& color = serial_color.rgb();
        return svg::Rgb (color.r(), color.g(), color.b());
    }else if (serial_color.has_rgba()) {
        const auto& color = serial_color.rgba();
        return svg::Rgba(color.rgb().r(), color.rgb().g(), color.rgb().b(), color.a());
    }
    return std::monostate();
}

RoutingSettings TransportDeserializer::ConvertRoutingSettings() const {
    RoutingSettings settings;
    settings.speed = serial_.transport_router().routing_settings().bus_velocity();
    settings.wait_time = serial_.transport_router().routing_settings().bus_wait_time();
    return settings;
}


Stop TransportDeserializer::SerialStopToStop(const serial::StopList::Stop& stop) const {
    Stop res;
    res.stop_name = stop.name();
    res.cooradinates = {stop.lat(), stop.lng()};
    return res;
}

Bus TransportDeserializer::SerialBusToBus(const serial::BusList::Bus& bus, const TransportCatalogue& c) const {
    Bus res;
    res.bus_name = bus.name();
    res.is_rounded = bus.is_rounded();
    std::vector<Stop*> stops_on_route;
    stops_on_route.reserve(bus.stops_id_size());

    for (const auto& s : bus.stops_id()) {
        std::string_view stop = id_to_stopname_.at(s);
        stops_on_route.push_back(&c.GetStop(stop));
    }
    res.stops_on_route = std::move(stops_on_route);
    return res;
}

std::pair<std::pair<std::string_view, std::string_view>, double> TransportDeserializer::NormalizeSerialStopToDist(
    const serial::StopsToDist& stops_to_dist,
    const TransportCatalogue& c) const
{

    std::pair<std::pair<std::string_view, std::string_view>, double> res;

    std::string_view first_name = id_to_stopname_.at(stops_to_dist.stops_between().first_stop());
    std::string_view last_name = id_to_stopname_.at(stops_to_dist.stops_between().last_stop());
    res = {{first_name, last_name}, stops_to_dist.dist()};
    return res;
}
