#pragma once

#include <string_view>
#include <string>
#include "geo.h"
#include <ostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include "domain.h"

using namespace std::literals;

class TransportCatalogue {
public:

	TransportCatalogue() = default;

	Stop* AddStop(const Stop& stop);

	Bus* AddBus(const Bus& bus);

	Bus& GetBus(std::string_view str) const ;

	Stop& GetStop(std::string_view str) const;

	const std::deque<Stop>& GetAllStops() const;

	const std::deque<Bus>& GetAllBuses() const;

	const std::unordered_set<Bus*>& GetBusesByStop(std::string_view stop) const;

	void AddDistance(std::string_view root_stop_name, const std::vector<std::pair<std::string_view, int>>& stopname_to_dist);

	int GetDistance(const std::pair<const Stop*, const Stop*> stops_between) const;

	void AddRouteLength(std::string_view name, RouteLengthInformation length);

	const RouteLengthInformation& GetRouteLength(std::string_view name) const;

	RouteLengthInformation ComputeRouteLength(const Bus& bus);

private:
	std::deque<Stop> stops_original_storage_;
	std::unordered_map<std::string_view, Stop*> stopname_to_stop;
	std::deque<Bus> buses_original_storage_;
	std::unordered_map<std::string_view, Bus*> busname_to_bus_;
	std::unordered_map<Stop*, std::unordered_set<Bus*>> stop_to_bus_;
	std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHasher> stops_to_distance_;
	std::unordered_map<std::string_view, RouteLengthInformation> route_length_information_;

	
};