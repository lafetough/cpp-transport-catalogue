#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include <optional>
#include "map_renderer.h"
#include "request_handler.h"

namespace json_reader{

	using namespace json;

	Document JsonRead(std::istream& json_input);

	void FillTransportCatalogue(const Document& json_input, TransportCatalogue& catalouge);

	Array HandleRequest(const Document& json_doc, RequestHandler& catalogue);

	Dict ReadMapRequest(const RequestHandler& rh);

	namespace json_create {

		Dict CreateJsonElem(const BusStat& stat, const int request_id);
		Dict CreateJsonElem(const std::unordered_set<Bus*>& buses, const int request_id);

		Dict NotFound(const int request_id);
	}
	void PrintAnswer(const Document& json_doc, RequestHandler& catalogue, std::ostream& out);

	renderer::Settings ReadSettings(const Document& json_doc);
}
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */