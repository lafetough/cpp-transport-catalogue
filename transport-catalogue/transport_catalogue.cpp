#include "transport_catalogue.h"
#include <iostream>
#include "geo.h"

std::deque<Stop> MakeOnlyUsedStops(const TransportCatalogue& catalogue_) {
    std::deque<Stop> used_stops;

    for (const Stop& stop : catalogue_.GetAllStops()) {
        if (catalogue_.GetBusesByStop(stop.stop_name).empty()) {
            continue;
        }
        used_stops.push_back(stop);
    }
    return used_stops;
}


Stop* TransportCatalogue::AddStop(const Stop& stop) {
    Stop* stop_ptr = &stops_original_storage_.emplace_back(stop);
    stopname_to_stop[stop_ptr->stop_name] = stop_ptr;
    stop_to_bus_[stop_ptr];
    return stop_ptr;
}


Bus* TransportCatalogue::AddBus(const Bus& bus) {
    Bus* bus_ptr = &buses_original_storage_.emplace_back(bus);
    busname_to_bus_[bus_ptr->bus_name] = bus_ptr;
    for (Stop*& stop : bus_ptr->stops_on_route) {
        if (stop_to_bus_.count(stop)) {
            stop_to_bus_[stop].insert(bus_ptr);
        }
    }
    return bus_ptr;
}

Bus& TransportCatalogue::GetBus(std::string_view str) const {
    using namespace std::literals;

    if (busname_to_bus_.count(str)) {
        return *(busname_to_bus_.at(str));
    }
    else {
        static Bus bus;
        return bus;
    }
}

Stop& TransportCatalogue::GetStop(std::string_view str) const {
    using namespace std::literals;
    return *(stopname_to_stop.at(str));

}

const std::deque<Stop>& TransportCatalogue::GetAllStops() const {
    return stops_original_storage_;
}

const std::deque<Bus>& TransportCatalogue::GetAllBuses() const {
    return buses_original_storage_;
}

const std::unordered_set<Bus*>& TransportCatalogue::GetBusesByStop(std::string_view stop) const {
    return stop_to_bus_.at(&GetStop(stop));
}

void TransportCatalogue::AddDistance(std::string_view root_stop_name, const std::vector<std::pair<std::string_view, double>>& stopname_to_dist) {
    for (const auto [name, dist] : stopname_to_dist) {
        std::pair<Stop*, Stop*> stops_pair = { &GetStop(root_stop_name), &GetStop(name) };
        stops_to_distance_[stops_pair] = dist;
    }
}
void TransportCatalogue::AddDistance(std::pair<std::string_view, std::string_view> stop_pair, double dist) {
    std::pair<Stop*, Stop*> stops_ptr = {&GetStop(stop_pair.first), &GetStop(stop_pair.second)};
    stops_to_distance_[stops_ptr] = dist;
}
void TransportCatalogue::AddDistance(TransportCatalogue::StopPairsToDist&& stops_to_dist) {
    stops_to_distance_ = std::move(stops_to_dist);
}


double TransportCatalogue::GetDistance(const std::pair<const Stop*, const Stop*> stops_between) const  {

    if (stops_to_distance_.count(stops_between)) {
        return stops_to_distance_.at(stops_between);
    }
    else {
        return stops_to_distance_.at({ stops_between.second, stops_between.first });
    }
}

void TransportCatalogue::AddRouteLength(std::string_view name, RouteLengthInformation length) {
    route_length_information_[name] = length;
}

const RouteLengthInformation& TransportCatalogue::GetRouteLength(std::string_view name) const {
    return route_length_information_.at(name);
}

RouteLengthInformation TransportCatalogue::ComputeRouteLength(const Bus& bus) {
    double geo_distance = 0.0;
    double total_route_lenght = 0.0;
    for (int i = 1; i < static_cast<int>(bus.stops_on_route.size()); ++i) {
        geo_distance += ComputeDistance(bus.stops_on_route[i - 1]->cooradinates, bus.stops_on_route[i]->cooradinates);
        total_route_lenght += GetDistance({ bus.stops_on_route[i - 1] , bus.stops_on_route[i] });
    }
    return { total_route_lenght, total_route_lenght / geo_distance };
}

const TransportCatalogue::StopPairsToDist& TransportCatalogue::GetStopsToDist() const {
    return stops_to_distance_;
}

const TransportCatalogue::BusnameToRouteInfo& TransportCatalogue::GetBusToRouteInfo() const {
    return route_length_information_;
}




