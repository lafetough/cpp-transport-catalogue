#include "transport_catalogue.h"
#include <iostream>
#include "geo.h"
#include "stat_reader.h"


bool Stop::empty() {
	return stop_name.empty();
}

Stop* TransportCatalogue::AddStop(Stop& stop) {
	Stop* stop_ptr = &stops_original_storage_.emplace_back(std::move(stop));
	stopname_to_stop[stop_ptr->stop_name] = stop_ptr;
	stop_to_bus_[stop_ptr];
	return stop_ptr;
}


Bus* TransportCatalogue::AddBus(Bus& bus) {
	Bus* bus_ptr = &buses_original_storage_.emplace_back(std::move(bus));
	busname_to_bus_[bus_ptr->bus_name] = bus_ptr;
	for (Stop*& stop : bus_ptr->stops_on_route) {
		if (stop_to_bus_.count(stop)) {
			stop_to_bus_[stop].push_back(bus_ptr);
		}
	}
	return bus_ptr;
}

Bus& TransportCatalogue::GetBus(std::string_view str) {
	using namespace std::literals;

	if (busname_to_bus_.count(str)) {
		return *(busname_to_bus_[str]);
	}
	else {
		static Bus bus;
		bus.bus_name = std::string(str);
		return bus;
	}
}

Stop& TransportCatalogue::GetStop(std::string_view str) {
	using namespace std::literals;
	try {
		return *(stopname_to_stop.at(str));
	}
	catch (...) {
		static Stop s;
		return s;
	}
}

std::ostream& TransportCatalogue::GetRouteInformation(std::ostream& os, std::string_view str) {
	Bus bus = GetBus(str);
	return output_processing::bus::PrintInformation(os, bus, *this);
}

const std::vector<Bus*>& TransportCatalogue::GetBusesByStop(Stop& stop) {
	try
	{
		return stop_to_bus_.at(&stop);
	}
	catch (...)
	{
		static std::vector<Bus*> empty_vec;
		return empty_vec;
	}
}

void TransportCatalogue::AddDistance(std::string_view root_stop_name, const std::vector<std::pair<std::string_view, int>>& stopname_to_dist) {
	for (const auto [name, dist] : stopname_to_dist) { 
		std::pair<Stop*, Stop*> stops_pair = { &GetStop(root_stop_name), &GetStop(name) };
		stops_to_distance_[stops_pair] = dist;
	}
}

int TransportCatalogue::GetDistance(const std::pair<Stop*, Stop*> stops_between) const  {
	try {
		return stops_to_distance_.at(stops_between);
	}
	catch (...) {
		return stops_to_distance_.at({ stops_between.second, stops_between.first });
	}
}	

void TransportCatalogue::AddRouteLength(std::string_view name, RouteLengthInformation length) {
	route_length_information_[name] = length;
}

const RouteLengthInformation& TransportCatalogue::GetRouteLength(std::string_view name) const {
	return route_length_information_.at(name);
}

