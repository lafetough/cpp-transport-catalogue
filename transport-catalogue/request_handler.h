#pragma once
#include "transport_catalogue.h"
#include <optional>
#include "map_renderer.h"
#include "transport_router.h"


//using BusStat = std::string;

class RequestHandler {
public:

    RequestHandler(const TransportCatalogue* tc, renderer::MapRenderer* mr, const TransportRouter* router )
        :tc_(tc), mr_(mr), router_(router)
    {}

    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    const std::unordered_set<Bus*>& GetBusesByStop(const std::string_view stop_name) const;

    svg::Document RenderMap() const;

    const std::optional<graph::Router<double>::RouteInfo> BuildRoute(const std::string_view from, const std::string_view to) const;

    const EdgeInfo& GetEdgeInfo(const graph::EdgeId id) const;

private:
    const TransportCatalogue* tc_;
    renderer::MapRenderer* mr_;
    const TransportRouter* router_;
};

