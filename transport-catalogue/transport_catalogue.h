#pragma once

#include <string_view>
#include <string>
#include <ostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include "domain.h"

using namespace std::literals;

class TransportCatalogue {
public:

    using StopPairsToDist = std::unordered_map<std::pair<const Stop*, const Stop*>, double, StopPairHasher>;
    using BusnameToRouteInfo = std::unordered_map<std::string_view, RouteLengthInformation>;

    TransportCatalogue() = default;

    Stop* AddStop(const Stop& stop);
    Bus* AddBus(const Bus& bus);

    void AddDistance(std::string_view root_stop_name, const std::vector<std::pair<std::string_view, double>>& stopname_to_dist);
    void AddDistance(std::pair<std::string_view, std::string_view> stop_pair, double dist);
    void AddDistance(StopPairsToDist&& stops_to_dist);

    void AddRouteLength(std::string_view name, RouteLengthInformation length);
    RouteLengthInformation ComputeRouteLength(const Bus& bus);

    //Геттеры
    Bus& GetBus(std::string_view str) const ;
    Stop& GetStop(std::string_view str) const;
    const std::deque<Stop>& GetAllStops() const;
    const std::deque<Bus>& GetAllBuses() const;
    double GetDistance(const std::pair<const Stop*, const Stop*> stops_between) const;
    const std::unordered_set<Bus*>& GetBusesByStop(std::string_view stop) const;
    const RouteLengthInformation& GetRouteLength(std::string_view name) const;
    const StopPairsToDist& GetStopsToDist() const;
    const BusnameToRouteInfo& GetBusToRouteInfo() const;


private:
    std::deque<Stop> stops_original_storage_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    std::deque<Bus> buses_original_storage_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<Stop*, std::unordered_set<Bus*>> stop_to_bus_;
    StopPairsToDist stops_to_distance_;
    BusnameToRouteInfo route_length_information_;
};


std::deque<Stop> MakeOnlyUsedStops(const TransportCatalogue& catalogue_);
