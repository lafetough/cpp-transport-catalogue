#include "json_reader.h"
#include "request_handler.h"
#include <sstream>
#include <algorithm>
#include "map_renderer.h"
#include "json_builder.h"

using namespace std::literals;

namespace json_reader {

	using namespace json;

	Document JsonRead(std::istream& json_input) {
		Document json_doc = Load(json_input);
		return json_doc;
	}

	void FillTransportCatalogue(const Document& json_doc, TransportCatalogue& catalogue) {

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

			catalogue.AddStop(stop);
		}

		// добавление расстояний
		for (const Node& node : base_array) {
			const Dict& dict = node.AsDict();
			if (dict.at("type").AsString() == "Stop") {
				std::vector<std::pair<std::string_view, int>> stopname_to_dist;

				const auto distances = dict.at("road_distances").AsDict();
				for (const auto& [station_name, distance] : distances) {
					stopname_to_dist.push_back({ station_name, distance.AsInt() });
				}
				catalogue.AddDistance(dict.at("name").AsString(), stopname_to_dist);
			}
		}

		//добавление автобусов 
		for (const Node& node : base_array) {
			const Dict& dict = node.AsDict();
			if (dict.at("type").AsString() == "Bus") {
				std::vector<Stop*> stops_ptr;
				for (const auto& stop : dict.at("stops").AsArray()) {
					stops_ptr.push_back(&catalogue.GetStop(stop.AsString()));
				}

				if (!dict.at("is_roundtrip").AsBool()) {
					stops_ptr.reserve(stops_ptr.size() * 2);
					for (auto it = stops_ptr.rbegin() + 1; it != stops_ptr.rend(); ++it) {
						stops_ptr.push_back(*it);
					}
				}
				Bus* bus_ptr = catalogue.AddBus({ dict.at("name").AsString(), stops_ptr, dict.at("is_roundtrip").AsBool() });
				catalogue.AddRouteLength(bus_ptr->bus_name, catalogue.ComputeRouteLength(*bus_ptr));
			}
		}

	}

	/*Array HandleRequest(const Document& json_doc, RequestHandler& handler) {

		Array requsets = json_doc.GetRoot().AsMap().at("stat_requests"s).AsArray();

		Array json_answer;

		for (const Node& node : requsets) {
			const Dict& request = node.AsMap();
			int request_id = request.at("id").AsInt();

			if (request.at("type"s).AsString() == "Bus"s) {
				const std::optional<BusStat>& stat = handler.GetBusStat(request.at("name"s).AsString());
				if (!stat) {
					json_answer.emplace_back(std::move(json_create::NotFound(request_id)));
					continue;
				}
				auto answer = json_create::CreateJsonElem(*stat, request_id);
				json_answer.emplace_back(std::move(answer));

			}
			else if (request.at("type"s).AsString() == "Stop"s) {

				try {
					const std::unordered_set<Bus*>& buses = handler.GetBusesByStop(request.at("name").AsString());
					Dict answer = json_create::CreateJsonElem(buses, request_id);
					json_answer.emplace_back(std::move(answer));
				}
				catch (...) {
					json_answer.emplace_back(std::move(json_create::NotFound(request_id)));
					continue;
				}
			}
			else {
				Dict map = ReadMapRequest(handler);
				map.insert({ "request_id"s, Node(request_id) });
				json_answer.emplace_back(std::move(map));
			}
		}
		return json_answer;
	}	*/

	json::Node HandleRequest(const Document& json_doc, RequestHandler& handler) {

		Array requsets = json_doc.GetRoot().AsDict().at("stat_requests"s).AsArray();

		Array json_answer;

		for (const Node& node : requsets) {
			const Dict& request = node.AsDict();
			int request_id = request.at("id").AsInt();

			if (request.at("type"s).AsString() == "Bus"s) {
				const std::optional<BusStat>& stat = handler.GetBusStat(request.at("name"s).AsString());
				if (!stat) {
					json_answer.emplace_back(std::move(json_create::NotFound(request_id)));
					continue;
				}
				Node answer = json_create::CreateJsonElem(*stat, request_id);
				json_answer.emplace_back(std::move(answer));

			}
			else if (request.at("type"s).AsString() == "Stop"s) {

				try {
					const std::unordered_set<Bus*>& buses = handler.GetBusesByStop(request.at("name").AsString());
					Node answer = json_create::CreateJsonElem(buses, request_id);
					json_answer.emplace_back(std::move(answer));
				}
				catch (...) {
					json_answer.emplace_back(std::move(json_create::NotFound(request_id)));
					continue;
				}
			}
			else {
				json::Node map = ReadMapRequest(handler, request_id);
				json_answer.emplace_back(std::move(map));
			}
		}
		return Builder{}.Value(json_answer).Build();
	}

	renderer::Settings ReadSettings(const Document& json_doc) {

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
			const auto& color = dict.at("underlayer_color");
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

	json::Node ReadMapRequest(const RequestHandler& rh, const int request_id) {

		std::ostringstream oss;
		const svg::Document constructed_map = rh.RenderMap();
		constructed_map.Render(oss);

		return Builder{}.StartDict().Key("map"s).Value(oss.str()).Key("request_id"s).Value(request_id).EndDict().Build();
	}


	namespace json_create {

		Node CreateJsonElem(const BusStat& stat, const int request_id) {
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

			/*answer["curvature"s] = Node(stat.curvature);
			answer["request_id"s] = Node(request_id);
			answer["route_length"s] = Node(stat.route_length);
			answer["stop_count"s] = Node(static_cast<int>(stat.stops_count));
			answer["unique_stop_count"] = Node(static_cast<int>(stat.unique_stops_count));*/
			return answer;
		}

		Node CreateJsonElem(const std::unordered_set<Bus*>& buses, const int request_id) {
			Array buses_ar;

			buses_ar.reserve(buses.size());
			for (const auto& bus : buses) {
				buses_ar.emplace_back(bus->bus_name);
			}
			//Array buses_ar_builded = const_cast<Array&>(buses_ar.EndArray().Build().AsArray());

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

		Node NotFound(const int request_id) {

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

	}


	void PrintAnswer(const Document& json_doc, RequestHandler& handler, std::ostream& out) {
		const Document doc = Document{ HandleRequest(json_doc, handler) };

		Print(doc, out);
	}
}