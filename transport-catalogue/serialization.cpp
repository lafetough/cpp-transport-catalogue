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

//    {
//        const auto& l = c.routing_settings();
//        std::cerr << "---------------" << std::endl;
//        std::cerr << "Routing settings: bus_velocity: "s << l.bus_velocity() << " wait_time: "s << l.bus_wait_time() << '\n';
//        std::cerr << "---------------" << std::endl;
//    }
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
        InsertEdgesInfo(i); //заполняем мапы
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
        std::string_view stop_name = catalogue_ref_.GetStop(wait.stop_name).stop_name;
        const auto& vertex_info = router_.GetStopVertexInfo(stop_name);
        serial_info.mutable_w_e()->set_stop_name(std::string(stop_name));
        serial_info.mutable_w_e()->set_time(wait.time);
        router_serial->mutable_edge_info_by_edge_id()->insert({edge_id, std::move(serial_info)});

        serial::StopGraphContain vertex_info_serial;
        vertex_info_serial.set_start_waiting(vertex_info.start_waiting);
        vertex_info_serial.set_end_waiting(vertex_info.end_waiting);
        router_serial->mutable_stops_vertex_cont()->insert({std::string(stop_name), std::move(vertex_info_serial)});

    }else if (std::holds_alternative<BusEdge>(edge_info)) {
        const auto& bus_edge = std::get<BusEdge>(edge_info);
        serial_info.mutable_b_e()->set_bus_name(std::string(bus_edge.bus_name));
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

DeserializedContent TransportDeserializer::Deserialize() {


    print(serial_);

    std::unique_ptr<TransportCatalogue> catalogue = std::make_unique<TransportCatalogue>(ConvertTC());
    std::unique_ptr<renderer::MapRenderer> map = std::make_unique<renderer::MapRenderer>(ConvertMap(*catalogue, ConvertMapSettings()));
    std::unique_ptr<TransportRouter> router = std::make_unique<TransportRouter>(ConvertTransportRouter(*catalogue));

//    DeserializedContent content{ConvertTC(), ConvertMap(*content.catalogue, ConvertMapSettings()), ConvertTransportRouter(*content.catalogue)};

    return {std::move(catalogue), std::move(map), std::move(router) };
}

TransportCatalogue TransportDeserializer::ConvertTC() {

    //std::cerr << "==========="s << std::endl;


    TransportCatalogue catalogue;
    //print(serial_);

    //Добавление остановок в stop_storage
    for (const auto& stop : serial_.stop_storage().stop_storage()) {
        Stop s = SerialStopToStop(stop);
        id_to_stopname_[stop.id()] = stop.name();
        catalogue.AddStop(std::move(s));
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

//Перенести--------------

template<typename Weight>
graph::Edge<Weight> SerialEdgeToNormal(const serial::Edge& edge_serial) {
    graph::Edge<Weight> edge_normal;
    edge_normal.from = edge_serial.from();
    edge_normal.to = edge_serial.to();
    edge_normal.is_waiting = edge_serial.is_waiting();
    edge_normal.weight = edge_serial.weight();
    return edge_normal;
}


//-----------------------

TransportRouter TransportDeserializer::ConvertTransportRouter(const TransportCatalogue& catalogue) const {
    const auto& edge_info_by_id = serial_.transport_router().edge_info_by_edge_id();
    std::unordered_map<graph::EdgeId, EdgeInfo> edge_info_by_id_hash;
    for (const auto& [edge_id, edge_info] : edge_info_by_id) {
        EdgeInfo temp;
        if (edge_info.has_b_e()) {
            BusEdge bus_edge_norm;
            bus_edge_norm.bus_name = edge_info.b_e().bus_name();
            bus_edge_norm.span_count = edge_info.b_e().span_count();
            bus_edge_norm.time = edge_info.b_e().time();
            temp = std::move(bus_edge_norm);
        }else if (edge_info.has_w_e()) {
            WaitEdge wait_edge_norm;
            wait_edge_norm.stop_name = edge_info.w_e().stop_name();
            wait_edge_norm.time = edge_info.w_e().time();
            temp = std::move(wait_edge_norm);
        }
        edge_info_by_id_hash[edge_id] = std::move(temp);
    }

    const auto& stops_vertex_cont = serial_.transport_router().stops_vertex_cont();
    std::unordered_map<std::string_view, StopGraphContain> stops_vertex_cont_hash;

    for (const auto& [stop_name, stop_vert_cont] : stops_vertex_cont) {
        const Stop& stop_ref = catalogue.GetStop(stop_name);
        StopGraphContain stop_vert = {stop_ref, stop_vert_cont.start_waiting(), stop_vert_cont.end_waiting()};
        stops_vertex_cont_hash.insert({stop_ref.stop_name, std::move(stop_vert)});
    }

    auto a = ConvertIternalData();

    TransportRouter tr(
        ConvertRoutingSettings(), catalogue,
        ConvertGraph(), std::move(a),
        std::move(edge_info_by_id_hash), std::move(stops_vertex_cont_hash));

    return tr;
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
            if (router_serial.routes_internal_data(i).value(k).IsInitialized()) {
                //std::optional<graph::Router<double>::RouteInternalData> temp;
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

//graph::Router<double> TransportDeserializer::ConvertRouter(const graph::DirectedWeightedGraph<double>& graph_ref) const {
//    const serial::Router router_serial = serial_.transport_router().router();
//    graph::Router<double>::RoutesInternalData data(
//        router_serial.routes_internal_data_size(),
//        std::vector<std::optional<graph::Router<double>::RouteInternalData>>(router_serial.routes_internal_data_size())
//        );

//    for (int i = 0; i < router_serial.routes_internal_data_size(); ++i) {
//        for (int k = 0; k < router_serial.routes_internal_data().at(i).value_size(); ++k) {
//            std::optional<graph::Router<double>::RouteInternalData> temp;
//            const auto& elem = router_serial.routes_internal_data(i).value(k);
//            temp->weight = elem.weight_val();
//            if (elem.has_prev_edge_id()) {
//                *temp->prev_edge = elem.prev_edge_id().value();
//            }
//            data[i][k] = std::move(temp);
//        }
//    }

//    graph::Router<double> router(graph_ref, std::move(data));
//    return std::move(router);
//}

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





























//void Serializator::Serialize(std::ostream& out) {
//    *serial_catalogue_.mutable_stop_storage() = json_ser_->ParseStopList();
//    *serial_catalogue_.mutable_bus_storage() = json_ser_->ParseBusList();
//    auto stops_to_dist_cont =  json_ser_->ParseDistance();

//    for (serial::StopsToDist& elem : stops_to_dist_cont) {
//        *serial_catalogue_.add_stops_to_dist() = std::move(elem);
//    }

//    try{
//        *serial_catalogue_.mutable_render_settings() = json_ser_->ParseRenderSettings();
//    }catch(std::out_of_range) {
//        *serial_catalogue_.mutable_render_settings() = {};
//    }
//    try {
//        *serial_catalogue_.mutable_routing_settings() = json_ser_->ParseRoutingSettings();
//    }catch(std::out_of_range) {
//        *serial_catalogue_.mutable_routing_settings() = {};
//    }

//    //print(serial_catalogue_);

//   // std::cerr << "Amount of stops at the end of serialization: "s << serial_catalogue_.stop_storage().stop_storage_size() << std::endl;
//    //std::cerr << "Amount of buses at the end of serialization: "s << serial_catalogue_.bus_storage().bus_storage_size() << std::endl;
//    serial_catalogue_.SerializeToOstream(&out);
//}



//TransportCatalogue Deserializator::Deserialize(std::istream& in) {
//    serial_catalogue_.ParseFromIstream(&in);

//    //std::cerr << "Amount of stops at the beginning of deserializetion: "s << serial_catalogue_.stop_storage().stop_storage_size() << std::endl;
//    //std::cerr << "Amount of buses at the beginning of deserializetion: "s << serial_catalogue_.bus_storage().bus_storage_size() << std::endl;

//    //print(serial_catalogue_);

////    std::cerr << "---------------"s << std::endl;
////    std::cerr << "Transport catalogue content"s << std::endl;


//    TransportCatalogue catalogue;

//    const serial::StopList& stop_storage = serial_catalogue_.stop_storage();
//    //const auto& id_stop_map = stop_storage.id_to_stop();
//    std::unordered_map<int, std::string_view> id_to_stopname;
//    std::unordered_map<std::string_view, int> stopname_to_id;

//    //Добавление остановок в stop_storage
//    for (const auto& stop : stop_storage.stop_storage()) {
//        catalogue.AddStop({stop.name(), Coordinates{stop.lat(), stop.lng()}});
//        id_to_stopname[stop.id()] = stop.name();
//        stopname_to_id[stop.name()] = stop.id();
//    }

////    const auto& a = catalogue.GetAllStops();
////    for (const auto& elem : a) {
////        std::cerr << "Stop: "s << elem.stop_name << std::endl;
////        //        std::cerr << "Buses to this stop: "s << std::endl;
////        //        for (const auto& f : catalogue.GetBusesByStop(elem.stop_name)) {
////        //            std::cerr << "    " << f->bus_name << std::endl;
////        //        }
////    }

//    //Добавление расстояний между остановками
//    const auto& stops_to_dist = serial_catalogue_.stops_to_dist();
//    for (const auto& elem : stops_to_dist) {
//        const auto& stop_id_pair = elem.stops_between();
//        std::string_view first_stop = id_to_stopname.at(stop_id_pair.first_stop());
//        std::string_view last_stop = id_to_stopname.at(stop_id_pair.last_stop());
//        catalogue.AddDistance({first_stop, last_stop}, elem.dist());
//    }

////    const auto& n = catalogue.GetStopsToDist();
////    for (const auto& [stop_pair, dist] : n) {
////        std::cerr << "Stop_pair: {"s << stop_pair.first->stop_name << " | "s << stop_pair.second->stop_name << "}, dist: "s << dist << std::endl;
////    }
////    std::cerr << "---------------"s << std::endl;

//    //Добавление автобусов(маршрутов) и их информация
//    const serial::BusList& bus_storage = serial_catalogue_.bus_storage();
//    for (const auto& bus : bus_storage.bus_storage()) {
////        std::cerr << "Added bus: Busname: "s << bus.name() << " Stops on route: "s << std::endl;

//        std::vector<Stop*> stops_in_route;
//        stops_in_route.reserve(bus.stops_id_size());

////        std::cerr << "Stops on route: "s << std::endl;

//        for (int i = 0; i < bus.stops_id_size(); ++i) {
//            std::string_view stop = id_to_stopname.at(bus.stops_id()[i]);
//            stops_in_route.push_back(&catalogue.GetStop(stop));
////            std::cerr << "    "s << stop << std::endl;
//        }

//        if (!bus.is_rounded()) {
//            stops_in_route.reserve(bus.stops_id_size() * 2);
//            for (int k = stops_in_route.size() - 2; k >= 0; --k) {
//                std::string_view s = id_to_stopname.at(bus.stops_id()[k]);
//                stops_in_route.push_back(&catalogue.GetStop(s));
////                std::cerr << "    "s << s << std::endl;
//            }
//        }

////        std::cerr << "Is_roundtrip: "s << bus.is_rounded() << std::endl;

//        const Bus* added_bus = catalogue.AddBus({bus.name(), std::move(stops_in_route), bus.is_rounded()});

//        //const auto& route_info = serial_.route_info().route_info().at(id);

//        RouteLengthInformation info = catalogue.ComputeRouteLength(*added_bus);

////        std::cerr << "Route info: route_length: "s << info.route_lenght << " curvuture: "s << info.curvature << std::endl;
//        catalogue.AddRouteLength(bus.name(), std::move(info));
////        std::cerr << "---------------"s << std::endl;
//    }



////    std::cerr << "Buses" << std::endl;
////    std::cerr << "---------------"s << std::endl;
////    const auto& b = catalogue.GetAllBuses();
////    for (const auto& elem : b) {
////        std::cerr << "Bus: "s << elem.bus_name << " Stops on route: "s << std::endl;
////        for(const auto stop : elem.stops_on_route) {
////            std::cerr << "    " << stop->stop_name << std::endl;
////        }
////    }



//    return catalogue;
//}



















