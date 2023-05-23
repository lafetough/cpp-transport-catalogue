#include "json_reader.h"
#include "request_handler.h"
#include <sstream>
#include <algorithm>
#include "map_renderer.h"
#include "json_builder.h"
#include <memory>

using namespace std::literals;

namespace json_reader {

    using namespace json;


    JSONHandler::JSONHandler(Document doc, TransportCatalogue* c)
        :json_doc(std::move(doc)), catalogue_(c)
	{
	}

    Document JsonRead(std::istream& json_input) {
		Document json_doc = Load(json_input);
		return json_doc;
	}

    void JSONHandler::FillTransportCatalogue() {

		const Array& base_array = json_doc.GetRoot().AsDict().at("base_requests"s).AsArray();

		// добавление остановок
		for (const Node& node : base_array) {
			const Dict& dict = node.AsDict();

			//пропускаем автобусы
			try {
				if (dict.at("type").AsString() != "Stop") {
					continue;
				}
			}
			catch (...) {
				throw std::invalid_argument("There are no this type identifaer");
			}

			Stop stop = { dict.at("name").AsString(), Coordinates {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()} };

            catalogue_->AddStop(stop);
		}

		// добавление расстояний
		for (const Node& node : base_array) {
			const Dict& dict = node.AsDict();
			if (dict.at("type").AsString() == "Stop") {
                std::vector<std::pair<std::string_view, double>> stopname_to_dist;

				const auto distances = dict.at("road_distances").AsDict();
				for (const auto& [station_name, distance] : distances) {
					stopname_to_dist.push_back({ station_name, distance.AsInt() });
                }
                catalogue_->AddDistance(dict.at("name").AsString(), stopname_to_dist);
			}
		}

		//добавление автобусов 
		for (const Node& node : base_array) {
			const Dict& dict = node.AsDict();
			if (dict.at("type").AsString() == "Bus") {
				std::vector<Stop*> stops_ptr;
                for (const auto& stop : dict.at("stops").AsArray()) {
                    stops_ptr.push_back(&catalogue_->GetStop(stop.AsString()));
				}

				if (!dict.at("is_roundtrip").AsBool()) {
					stops_ptr.reserve(stops_ptr.size() * 2);
					for (auto it = stops_ptr.rbegin() + 1; it != stops_ptr.rend(); ++it) {
						stops_ptr.push_back(*it);
					}
				}
                Bus* bus_ptr = catalogue_->AddBus({ dict.at("name").AsString(), stops_ptr, dict.at("is_roundtrip").AsBool() });
                catalogue_->AddRouteLength(bus_ptr->bus_name, catalogue_->ComputeRouteLength(*bus_ptr));
			}
		}

	}

    json::Node JSONHandler::HandleRequest(RequestHandler& handler) {

		Array requsets = json_doc.GetRoot().AsDict().at("stat_requests"s).AsArray();

		Array json_answer;

		for (const Node& node : requsets) {
			const Dict& request = node.AsDict();
			int request_id = request.at("id").AsInt();

			if (request.at("type"s).AsString() == "Bus"s) {
				const std::optional<BusStat>& stat = handler.GetBusStat(request.at("name"s).AsString());
				if (!stat) {
					json_answer.emplace_back(std::move(NotFound(request_id)));
					continue;
				}
				Node answer = CreateJsonElem(*stat, request_id);
				json_answer.emplace_back(std::move(answer));

			}
			else if (request.at("type"s).AsString() == "Stop"s) {

				try {
					const std::unordered_set<Bus*>& buses = handler.GetBusesByStop(request.at("name").AsString());
					Node answer = CreateJsonElem(buses, request_id);
					json_answer.emplace_back(std::move(answer));
				}
				catch (...) {
					json_answer.emplace_back(std::move(NotFound(request_id)));
					continue;
				}
			}
			else if (request.at("type"s).AsString() == "Route"s) {

				const auto route_info = handler.BuildRoute(request.at("from"s).AsString(), request.at("to"s).AsString());
				if (!route_info) {
					Node answer = NotFound(request_id);
					json_answer.push_back(std::move(answer));
					continue;
				}

				Node answer = CreateJsonRouteAnswer(handler, *route_info, request_id);
				json_answer.push_back(std::move(answer));
			}
			else {
				json::Node map = ReadMapRequest(handler, request_id);
				json_answer.emplace_back(std::move(map));
			}
		}
		return Builder{}.Value(json_answer).Build();
	}

    renderer::Settings JSONHandler::ReadWriteSettings() {

		const Dict& dict = json_doc.GetRoot().AsDict().at("render_settings").AsDict();

		renderer::Settings settings;

		settings.width = dict.at("width"s).AsDouble();
		settings.height = dict.at("height"s).AsDouble();
		settings.padding = dict.at("padding"s).AsDouble();
		settings.line_width = dict.at("line_width"s).AsDouble();
		settings.stop_radius = dict.at("stop_radius"s).AsDouble();
		settings.bus_label_font_size = dict.at("bus_label_font_size"s).AsInt();
		settings.bus_label_offset = {
			dict.at("bus_label_offset"s).AsArray()[0].AsDouble(),
			dict.at("bus_label_offset"s).AsArray()[1].AsDouble()
		};
		settings.stop_label_font_size = dict.at("stop_label_font_size"s).AsInt();
		settings.stop_label_offset = {
			dict.at("stop_label_offset"s).AsArray()[0].AsDouble(),
			dict.at("stop_label_offset"s).AsArray()[1].AsDouble()
		};

		{
            const auto& color = dict.at("underlayer_color"s);
			if (color.IsString()) {
				settings.underlayer_color = color.AsString();
			}
			else if (color.IsArray()) {
				const auto& color_ar = color.AsArray();
				if (color_ar.size() == 3) {
					settings.underlayer_color = svg::Rgb{
						static_cast<uint8_t>(color_ar[0].AsInt()),
						static_cast<uint8_t>(color_ar[1].AsInt()),
						static_cast<uint8_t>(color_ar[2].AsInt())
					};
				}
				else if (color_ar.size() == 4) {
                    std::cerr << color_ar[3].AsDouble() << std::endl;
					settings.underlayer_color = svg::Rgba{
						static_cast<uint8_t>(color_ar[0].AsInt()),
						static_cast<uint8_t>(color_ar[1].AsInt()),
						static_cast<uint8_t>(color_ar[2].AsInt()), color_ar[3].AsDouble()
					};
				}
			}
		}

		settings.underlayer_width = dict.at("underlayer_width"s).AsDouble();

		{
			const auto& palette = dict.at("color_palette"s).AsArray();
			settings.color_palette.reserve(palette.size());
			for (const auto& color : palette) {
				if (color.IsString()) {
					settings.color_palette.push_back(color.AsString());
				}
				else if (color.IsArray()) {
					const auto& color_ar = color.AsArray();
					if (color_ar.size() == 3) {
						settings.color_palette.push_back(svg::Rgb{
							static_cast<uint8_t>(color_ar[0].AsInt()),
							static_cast<uint8_t>(color_ar[1].AsInt()),
							static_cast<uint8_t>(color_ar[2].AsInt())
							});
					}
					else if (color_ar.size() == 4) {
						settings.color_palette.push_back(svg::Rgba{
							static_cast<uint8_t>(color_ar[0].AsInt()),
							static_cast<uint8_t>(color_ar[1].AsInt()),
							static_cast<uint8_t>(color_ar[2].AsInt()),
							color_ar[3].AsDouble()
							});
					}
				}
			}
		}
		return settings;
	}

    json::Node JSONHandler::ReadMapRequest(const RequestHandler& rh, const int request_id) const {

		std::ostringstream oss;
		const svg::Document constructed_map = rh.RenderMap();
		constructed_map.Render(oss);

		return Builder{}.StartDict().Key("map"s).Value(oss.str()).Key("request_id"s).Value(request_id).EndDict().Build();
	}

    RoutingSettings JSONHandler::ReadRoutingSettings() const {

        if (!json_doc.GetRoot().AsDict().count("routing_settings"s)) {
            return {};
        }

		const Dict& settings = json_doc.GetRoot().AsDict().at("routing_settings"s).AsDict();

		return {settings.at("bus_velocity"s).AsDouble(), settings.at("bus_wait_time"s).AsDouble()};

	}

    Node JSONHandler::CreateJsonRouteAnswer(const RequestHandler& handler, const graph::Router<double>::RouteInfo& route_info, const int request_id) const {

		std::unique_ptr<Builder> answer = std::make_unique<Builder>();
		answer->StartDict()
			.Key("request_id"s)
			.Value(request_id)
			.Key("total_time"s)
			.Value(route_info.weight)
			.Key("items"s)
			.StartArray();

		//std::vector<Node> items;

		for (const graph::EdgeId edge_id : route_info.edges) {
			const EdgeInfo info = handler.GetEdgeInfo(edge_id);
			if (std::holds_alternative<WaitEdge>(info)) {
				const auto wait_info = std::get<WaitEdge>(info);
				answer->StartDict()
					.Key("type"s)
					.Value("Wait"s)
					.Key("stop_name"s)
					.Value(std::string(wait_info.stop_name))
					.Key("time"s)
					.Value(wait_info.time)
					.EndDict();
				//items.push_back(std::move(n)); 
			}
			else if (std::holds_alternative<BusEdge>(info)) {
				const auto bus_info = std::get<BusEdge>(info);
				answer->StartDict()
					.Key("type"s)
					.Value("Bus"s)
					.Key("bus"s)
					.Value(std::string(bus_info.bus_name))
					.Key("span_count"s)
					.Value(bus_info.span_count)
					.Key("time"s)
					.Value(bus_info.time)
					.EndDict();
					//.Build();
				//items.push_back(std::move(n));
			}
			else {
				throw;
			}
		}

		return answer->EndArray().EndDict().Build();

	}

    Node JSONHandler::CreateJsonElem(const BusStat& stat, const int request_id) const {
		Node answer = json::Builder{}
			.StartDict()
			.Key("curvature"s)
			.Value(stat.curvature)
			.Key("request_id"s)
			.Value(request_id)
			.Key("route_length"s)
			.Value(stat.route_length)
			.Key("stop_count"s)
			.Value(static_cast<int>(stat.stops_count))
			.Key("unique_stop_count"s)
			.Value(static_cast<int>(stat.unique_stops_count))
			.EndDict()
			.Build()
			;
		return answer;
	}

    Node JSONHandler::CreateJsonElem(const std::unordered_set<Bus*>& buses, const int request_id) const {
		Array buses_ar;

		buses_ar.reserve(buses.size());
		for (const auto& bus : buses) {
			buses_ar.emplace_back(bus->bus_name);
		}

		std::sort(buses_ar.begin(), buses_ar.end(), [](const Node& lhs, const Node& rhs) {
			return lhs.AsString() < rhs.AsString();
			});
		Node answer = Builder{}
			.StartDict()
			.Key("buses"s)
			.Value(buses_ar)
			.Key("request_id"s)
			.Value(request_id)
			.EndDict()
			.Build()
			;
		return answer;
	}

    Node JSONHandler::NotFound(const int request_id) const {
		Node answer = Builder{}
			.StartDict()
			.Key("error_message"s)
			.Value("not found"s)
			.Key("request_id"s)
			.Value(request_id)
			.EndDict()
			.Build()
			;
		return answer;
	}

    void JSONHandler::PrintAnswer(RequestHandler& handler, std::ostream& out) {
        const Document doc = Document{ HandleRequest(handler) };
		Print(doc, out);
	}

    const Document& JSONHandler::GetJsonDoc() const {
        return json_doc;
    }

    SerializingSettings JSONHandler::ParseSerializeSettings() {
        SerializingSettings settings;
        const Dict& serial_settings = json_doc.GetRoot().AsDict().at("serialization_settings"s).AsDict();
        settings.file_name = serial_settings.at("file"s).AsString();

        return settings;
    }


    //JSONSerializer-----------------------------------------------

    serial::StopList JSONSerializer::ParseStopList() {
        serial::StopList stops_serialized;

        const Array& base_array = json_doc_.GetRoot().AsDict().at("base_requests"s).AsArray();
        unsigned int id = 0;
        for (const Node& elem : base_array) {
            const Dict& request = elem.AsDict();

            if (request.at("type"s).AsString() != "Stop"s) {
                continue;
            }

            //std::cerr << "Handaling stop: "s << request.at("name"s).AsString() << ", lat: "s << request.at("latitude"s).AsDouble() << " lng: "s << request.at("longitude"s).AsDouble() << " ID: "s << id << std::endl;

            serial::StopList::Stop stop;
            stop.set_name(request.at("name"s).AsString());
            stop.set_lat(request.at("latitude"s).AsDouble());
            stop.set_lng(request.at("longitude"s).AsDouble());
            stop.set_id(id);

            //std::cerr << "Serialized: name: "s << stop.name() << " lat: "s << stop.lat() << " lng: "s << stop.lng() << std::endl;

            id_to_stop_name_[id] = stop.name();
            stop_name_to_id_[stop.name()] = id;

            stops_serialized.mutable_stop_storage()->Add(std::move(stop));

            //std::cerr << "Id_to_stop: id_to_stop(name): "s << stops_serialized.id_to_stop().at(id).name() << " stopname_to_id(id): "s << stops_serialized.stopname_to_id().at(request.at("name"s).AsString()) << std::endl;

            ++id;
        }
        //std::cout << stops_serialized.stopname_to_id().at("Universam"s) << std::endl;
        return stops_serialized;
    }

    serial::BusList JSONSerializer::ParseBusList() {
        serial::BusList bus_serialized;

        const Array& base_array = json_doc_.GetRoot().AsDict().at("base_requests"s).AsArray();
        unsigned int id = 0;

        for (const Node& elem : base_array) {
            const Dict& request = elem.AsDict();

            if (request.at("type").AsString() != "Bus"s) {
                continue;
            }

            serial::BusList::Bus bus;
            //bus_serialized.mutable_busname_to_id()->insert({request.at("name"s).AsString(), id});

            bus.set_name(request.at("name"s).AsString());
            bus.set_id(id);
            const auto& stops = request.at("stops"s).AsArray();

            for (const auto& stop : stops) {
                int stop_id = stop_name_to_id_.at(stop.AsString());
                bus.add_stops_id(stop_id);
            }
            bus.set_is_rounded(request.at("is_roundtrip"s).AsBool());

            bus_serialized.mutable_bus_storage()->Add(std::move(bus));

            ++id;
        }
        return bus_serialized;
    }

    std::vector<serial::StopsToDist> JSONSerializer::ParseDistance() const {
        std::vector<serial::StopsToDist> stops_to_dist;
        const Array& base_array = json_doc_.GetRoot().AsDict().at("base_requests"s).AsArray();

        for (const Node& elem : base_array) {
            const Dict& request = elem.AsDict();

            if (request.at("type"s).AsString() != "Stop"s) {
                continue;
            }

            const std::string& curr_stop_name = request.at("name"s).AsString();

            const Dict& _to_stop_dist = request.at("road_distances"s).AsDict();

            for (const auto& [name, dist_node] : _to_stop_dist) {
                serial::StopsToDist sd;
                sd.mutable_stops_between()->set_first_stop(stop_name_to_id_.at(curr_stop_name));
                sd.mutable_stops_between()->set_last_stop(stop_name_to_id_.at(name));
                sd.set_dist(dist_node.AsDouble());
                stops_to_dist.push_back(std::move(sd));
            }

        }
        return stops_to_dist;
    }

    SerializingSettings ParseSerializeSettings(const Document& json_doc) {
        SerializingSettings settings;
        const Dict& serial_settings = json_doc.GetRoot().AsDict().at("serialization_settings"s).AsDict();
        settings.file_name = serial_settings.at("file"s).AsString();

        return settings;
    }

    serial::RenderSettings JSONSerializer::ParseRenderSettings() {

        serial::RenderSettings serial_render_settings;
        const Dict& render_settings = json_doc_.GetRoot().AsDict().at("render_settings"s).AsDict();

        serial_render_settings.set_width(render_settings.at("width"s).AsDouble());
        serial_render_settings.set_height(render_settings.at("height"s).AsDouble());
        serial_render_settings.set_padding(render_settings.at("padding"s).AsDouble());
        serial_render_settings.set_line_width(render_settings.at("line_width"s).AsDouble());
        serial_render_settings.set_stop_radius(render_settings.at("stop_radius"s).AsDouble());
        serial_render_settings.set_stop_label_font_size(render_settings.at("bus_label_font_size"s).AsInt());

        serial::Offset b_off;
        b_off.set_x(render_settings.at("bus_label_offset"s).AsArray()[0].AsDouble());
        b_off.set_y(render_settings.at("bus_label_offset"s).AsArray()[1].AsDouble());
        *serial_render_settings.mutable_bus_label_offset() = std::move(b_off);

        serial_render_settings.set_bus_label_font_size(render_settings.at("stop_label_font_size"s).AsInt());

        serial::Offset s_off;
        s_off.set_x(render_settings.at("stop_label_offset"s).AsArray()[0].AsDouble());
        s_off.set_x(render_settings.at("stop_label_offset"s).AsArray()[1].AsDouble());
        *serial_render_settings.mutable_stop_label_offset() = std::move(s_off);

        *serial_render_settings.mutable_underlayer_color() = ParseColor(render_settings.at("underlayer_color"s));
        serial_render_settings.set_underlayer_width(render_settings.at("underlayer_width"s).AsDouble());

        for (const auto& color : render_settings.at("color_palette"s).AsArray()) {
            serial_render_settings.mutable_color_palette()->Add(ParseColor(color));
        }

        return serial_render_settings;

    }

    serial::RoutingSettings JSONSerializer::ParseRoutingSettings() {
        const Dict& route_settings = json_doc_.GetRoot().AsDict().at("routing_settings"s).AsDict();
        serial::RoutingSettings routing_serial;

        routing_serial.set_bus_velocity(route_settings.at("bus_velocity"s).AsInt());
        routing_serial.set_bus_wait_time(route_settings.at("bus_wait_time"s).AsInt());

        return routing_serial;

    }


    serial::Color JSONSerializer::ParseColor(const Node& node) {
        serial::Color color;
        if (node.IsString()) {
            serial::SolidColor solid_c;
            solid_c.set_color(node.AsString());
            *color.mutable_solid_color() = std::move(solid_c);
        }else if (node.IsArray() && node.AsArray().size() == 4) {
            const auto& color_arr = node.AsArray();
            serial::RGBA rgba_c;
            rgba_c.mutable_rgb()->set_r(color_arr[0].AsInt());
            rgba_c.mutable_rgb()->set_g(color_arr[1].AsInt());
            rgba_c.mutable_rgb()->set_b(color_arr[2].AsInt());
            rgba_c.set_a(color_arr[3].AsDouble());
            *color.mutable_rgba() = std::move(rgba_c);
        }else if (node.IsArray()) {
            const auto& color_arr = node.AsArray();
            serial::RGB rgb_c;
            rgb_c.set_r(color_arr[0].AsInt());
            rgb_c.set_g(color_arr[1].AsInt());
            rgb_c.set_b(color_arr[2].AsInt());
            *color.mutable_rgb() = std::move(rgb_c);
        }
        return color;
    }


    const Document& JSONSerializer::GetJsonDoc() const {
        return json_doc_;
    }

}



















