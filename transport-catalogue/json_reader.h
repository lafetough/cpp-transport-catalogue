#pragma once
#include "transport_catalogue.h"
#include "json.h"
#include <optional>
#include "map_renderer.h"
#include "request_handler.h"
#include <transport_catalogue.pb.h>

namespace json_reader {

	using namespace json;

    Document JsonRead(std::istream& json_input);


    class JSONHandler {
    public:
        JSONHandler(Document doc, TransportCatalogue* c = nullptr);

        void FillTransportCatalogue();
        json::Node HandleRequest(RequestHandler& handler);
        renderer::Settings ParseRenderSettings();
		json::Node ReadMapRequest(const RequestHandler& rh, const int request_id) const;
        RoutingSettings ReadRoutingSettings() const;
        void PrintAnswer(RequestHandler& handler, std::ostream& out);

        const Document& GetJsonDoc() const;

	private:
        TransportCatalogue* catalogue_;
        const Document json_doc;

		Node CreateJsonElem(const BusStat& stat, const int request_id) const;
		Node CreateJsonElem(const std::unordered_set<Bus*>& buses, const int request_id) const;
		Node CreateJsonRouteAnswer(const RequestHandler& handler, const graph::Router<double>::RouteInfo& route_info, const int request_id) const;

		Node NotFound(const int request_id) const;
    };

    //Serial ========================

    SerializingSettings ParseSerializeSettings(const Document& json_doc);

}
