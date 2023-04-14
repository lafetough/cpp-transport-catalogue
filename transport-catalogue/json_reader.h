#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include <optional>
#include "map_renderer.h"
#include "request_handler.h"

namespace json_reader {

	using namespace json;

	//MapRanderer и RequestHandler сделать полями класса не получилось, т.к. они инициализируются по прочитаному JSON'у

	class JSONReader {
	public:
		JSONReader(TransportCatalogue& c);

		Document JsonRead(std::istream& json_input) const;

		void FillTransportCatalogue(const Document& json_input);

		json::Node HandleRequest(const Document& json_doc, RequestHandler& catalogue);

		renderer::Settings ReadWriteSettings(const Document& json_doc);
		
		json::Node ReadMapRequest(const RequestHandler& rh, const int request_id) const;

		RoutingSettings ReadRoutingSettings(const Document& json_doc) const;

		void PrintAnswer(const Document& json_doc, RequestHandler& handler, std::ostream& out);

	private:
		TransportCatalogue& catalogue_;

		Node CreateJsonElem(const BusStat& stat, const int request_id) const;
		Node CreateJsonElem(const std::unordered_set<Bus*>& buses, const int request_id) const;

		Node CreateJsonRouteAnswer(const RequestHandler& handler, const graph::Router<double>::RouteInfo& route_info, const int request_id) const;

		Node NotFound(const int request_id) const;
	};
	
}
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */