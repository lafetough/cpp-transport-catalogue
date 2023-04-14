#include "request_handler.h"
#include "svg.h"



const std::unordered_set<Bus*>& RequestHandler::GetBusesByStop(const std::string_view stop_name) const {
	return tc_.GetBusesByStop(stop_name);
}

std::optional<BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const {
    const Bus& bus = tc_.GetBus(bus_name);
    if (bus.stops_on_route.size() == 0) {
        return {};
    }

    std::unordered_set<Stop*> unique_stops(bus.stops_on_route.begin(), bus.stops_on_route.end());
    const auto& route = tc_.GetRouteLength(bus.bus_name);
    BusStat stat = {
        route.curvature,
        //0,
        route.route_lenght,
        bus.stops_on_route.size(),
        unique_stops.size()
    };
    return stat;
}

svg::Document RequestHandler::RenderMap() const {
    return mr_.RenderObj(tc_);
}

const std::optional<graph::Router<double>::RouteInfo> RequestHandler::BuildRoute(const std::string_view from, const std::string_view to) const {
    return router_.Build(router_.GetStopVertexInfo(from).start_waiting, router_.GetStopVertexInfo(to).start_waiting);
    //return router_.Build(from, to);
}

const EdgeInfo& RequestHandler::GetEdgeInfo(const graph::EdgeId id) const {
    return router_.GetEdgeInfoByEdgeId(id);
}

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */