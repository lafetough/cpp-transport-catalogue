#pragma once

#include <string_view>
#include <string>
#include "geo.h"
#include <ostream>
#include <vector>
#include <deque>
#include <unordered_map>


struct Stop
{

	std::string stop_name;
	Coordinates cooradinates;
	bool operator==(const Stop& other) {
		return stop_name == other.stop_name && cooradinates == other.cooradinates;
	}

	bool empty();
};

struct StopPairHasher
{
	static const int N = 42;
	std::size_t operator() (const std::pair<Stop*, Stop*> pr1) const {
		size_t h1 = hasher(pr1.first->cooradinates.lat * N);
		size_t h2 = hasher(pr1.first->cooradinates.lng * (N * N));
		size_t h3 = hasher(pr1.second->cooradinates.lat * (N * N * N));
		size_t h4 = hasher(pr1.second->cooradinates.lng * (N * N * N * N));
		return h1 + h2 + h3 + h4;
	}
	std::hash<double> hasher;
};

struct Bus
{

	std::string bus_name;
	std::vector<Stop*> stops_on_route;

	bool operator==(const Bus& other) {
		return bus_name == other.bus_name && stops_on_route == other.stops_on_route;
	}
};




class TransportCatalogue {
public:

	TransportCatalogue() = default;

	Stop* AddStop(Stop stop);

	Bus* AddBus(Bus&& bus);

	Bus& GetBus(std::string_view str);

	Stop& GetStop(std::string_view str);

	std::ostream& GetRouteInformation(std::ostream& os, std::string_view str);

	std::vector<Bus*>& GetBusesByStop(Stop& stop);

	void AddDistance(std::string_view root_stop_name, std::vector<std::pair<std::string_view, int>> stopname_to_dist);

	int GetDistance(const std::pair<Stop*, Stop*> stops_between) const;

	



	std::deque<Bus> _GetBuses() const;

	std::unordered_map<std::string_view, Bus*> _GetBusesMap() const;

	std::deque<Stop> _GetStops() const;

	std::unordered_map<std::string_view, Stop*> _GetMap() const;

	const std::unordered_map<std::pair<Stop*, Stop*>, int, StopPairHasher>& _GetStopsToDist() const;

private:

	std::deque<Stop> stops_original_storage_;
	std::unordered_map<std::string_view, Stop*> stopname_to_stop;
	std::deque<Bus> buses_original_storage_;
	std::unordered_map<std::string_view, Bus*> busname_to_bus_;
	std::unordered_map<Stop*, std::vector<Bus*>> stop_to_bus_;
	std::unordered_map<std::pair<Stop*, Stop*>, int, StopPairHasher> stops_to_distance_;

};

