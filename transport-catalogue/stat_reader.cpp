#include "stat_reader.h"
#include <unordered_set>
#include "input_reader.h"
#include <algorithm>
#include <execution>


std::ostream& output_processing::bus::Information(std::ostream& os, const Bus& bus, TransportCatalogue& catalouge) {
	if (bus.stops_on_route.empty()) {
		os << "Bus " << bus.bus_name << ": not found" << std::endl;
		return os;
	}
	std::unordered_set<Stop*> unique(bus.stops_on_route.begin(), bus.stops_on_route.end());
	os << "Bus " << bus.bus_name.data() << ": " << bus.stops_on_route.size() << " stops on route, ";
	os << unique.size() << " unique stops, ";
	double geo_distance = 0.0;
	int total_route_lenght = 0;
	for (int i = 0; i < bus.stops_on_route.size() - 1; ++i) {
		geo_distance += ComputeDistance(bus.stops_on_route[i]->cooradinates, bus.stops_on_route[i + 1]->cooradinates);
		total_route_lenght += catalouge.GetDistance({ bus.stops_on_route[i] , bus.stops_on_route[i + 1] });
	}
	os << total_route_lenght << " route length, " << total_route_lenght / geo_distance << " curvature" << std::endl;

	return os;
}

output_processing::Instruction output_processing::Identifaer(std::string_view str) {
	size_t first_sign = str.find_first_not_of(' ');
	auto command = str.substr(first_sign, str.find_first_of(' ', first_sign) - first_sign);
	if (command == "Stop") {
		return output_processing::Instruction::STOP;
	}
	else {
		return output_processing::Instruction::BUS;
	}
}

std::string output_processing::NameParsing(std::string_view str) {
	size_t begin = str.find_first_not_of(' ');
	size_t end = str.find(' ', begin);
	str.remove_prefix(end);
	begin = str.find_first_not_of(' ');
--	end = str.find_last_of(' ', begin);
	return std::string(str.substr(begin, end - begin));
}

std::ostream& output_processing::stop::Information(std::ostream& os, std::string_view& stop, TransportCatalogue& catalouge) {
	using namespace std::literals;
	std::string name = NameParsing(stop);
	Stop& stop_getted = catalouge.GetStop(name);
	os << "Stop "s << name << ": "s;
	if (stop_getted.empty()) {
		os << "not found"s << std::endl;
		return os;
	}
	std::vector<Bus*> buses = catalouge.GetBusesByStop(stop_getted);
	if (buses.empty()) {
		os << "no buses"s << std::endl;
		return os; 
	}
	std::sort(
		std::execution::par,
		buses.begin(),
		buses.end(),
		[](Bus* lhs, Bus* rhs) {
			return lhs->bus_name < rhs->bus_name;
		}
	);
	buses.erase(std::unique(
		std::execution::par,
		buses.begin(),
		buses.end()
	), buses.end());

	os << "buses"s;
	for (const Bus* bus : buses) {
		os << " "s << bus->bus_name;
	}
	os << std::endl;
	return os;
}