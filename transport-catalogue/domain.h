#pragma once
#include "geo.h"
#include <string>
#include <vector>
#include "graph.h"

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
	std::size_t operator() (const std::pair<const Stop*,  const Stop*> pr1) const;
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
	double curvature;
	int route_length;
	size_t stops_count;
	size_t unique_stops_count;
};

struct StopGraphContain {

	const Stop& stop;
	graph::VertexId start_waiting;
	graph::VertexId end_waiting;

};

struct RoutingSettings {
	double speed;
	double wait_time;
};

struct WaitEdge {
	std::string_view stop_name;
	double time;
};

struct BusEdge
{
	std::string_view bus_name;
	int span_count;
	double time;
};
