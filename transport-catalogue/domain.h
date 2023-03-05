#pragma once
#include "geo.h"
#include <string>
#include <vector>

using namespace geo;

struct Stop
{
	std::string stop_name;
	Coordinates cooradinates;
	bool operator==(const Stop& other) const;

	bool empty() const;
};

struct StopPairHasher
{
	static const int N = 42;
	std::size_t operator() (const std::pair<Stop*, Stop*> pr1) const;
	std::hash<double> hasher;
};

struct Bus
{
	std::string bus_name;
	std::vector<Stop*> stops_on_route;
	bool is_rounded;

	bool operator==(const Bus& other) const;
};

struct RouteLengthInformation {
	int route_lenght;
	double curvature;

	bool operator==(const RouteLengthInformation& other) const {
		return route_lenght == other.route_lenght && curvature == other.curvature;
	}

};

struct BusStat
{
	double curvuture;
	int route_length;
	size_t stops_count;
	size_t unique_stops_count;
};


