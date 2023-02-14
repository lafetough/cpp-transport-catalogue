#include "input_reader.h"
#include <iostream>
#include <execution>
#include <algorithm>

std::string input_processing::thread_reading::ReadLine(std::istream& in) {
	std::string s;
	std::getline(in, s);
	return s;
}
int input_processing::thread_reading::ReadLineWithNumber(std::istream& in) {
	int result = 0;
	in >> result;
	ReadLine(in);
	return result;
}

RouteLengthInformation input_processing::bus_parsing::ComputeRouteLength(const Bus& bus, TransportCatalogue& catalouge) {
	double geo_distance = 0.0;
	int total_route_lenght = 0;
	for (int i = 0; i < bus.stops_on_route.size() - 1; ++i) {
		geo_distance += ComputeDistance(bus.stops_on_route[i]->cooradinates, bus.stops_on_route[i + 1]->cooradinates);
		total_route_lenght += catalouge.GetDistance({ bus.stops_on_route[i] , bus.stops_on_route[i + 1] });
	}
	return { total_route_lenght, total_route_lenght / geo_distance };
}

void input_processing::FillTransportCatalogue(const std::vector<std::string>& requests, TransportCatalogue& catalouge) {
	//добавление остановок 

	for (std::string_view sv : requests) {
		if (ParseInstructionType(sv) == Instruction::STOP) {
			auto stop = stop_parsing::ParseStop(sv);
			catalouge.AddStop(stop);
		}
	}

	// добавление расстояний
	for (std::string_view sv : requests) {
		if (ParseInstructionType(sv) == Instruction::STOP) {
			catalouge.AddDistance(ParseName(sv), stop_parsing::ParseDistances(sv));
		}
	}
	// добавление маршрутов
	for (std::string_view sv : requests) {
		Instruction identifaer = ParseInstructionType(sv);
		if (identifaer != Instruction::STOP) {
			auto bus = bus_parsing::ParseBus(sv, identifaer, catalouge);
			Bus* bus_ptr = catalouge.AddBus(bus);
			catalouge.AddRouteLength(bus_ptr->bus_name, bus_parsing::ComputeRouteLength(*bus_ptr, catalouge));
		}
	}
}

input_processing::Instruction input_processing::ParseInstructionType(std::string_view str) {
	using namespace input_processing;
	size_t first_sign = str.find_first_not_of(' ');		
	auto command = str.substr(first_sign, str.find_first_of(' ', first_sign) - first_sign);
	if (command == "Stop") {
		return Instruction::STOP;
	}
	if (command == "Bus") {
		if (str.find('>') != std::string_view::npos) {
			return Instruction::BUS_CIRCLE;
		}
		else if(str.find('-') != std::string_view::npos) {
			return Instruction::BUS_RETURNING;
		}
	}
	throw std::invalid_argument("Invalid cammand");
}

std::string_view input_processing::ParseName(std::string_view& str) {
	str.remove_prefix(str.find_first_not_of(' ')); //Удаляем пробелы вначале
	str.remove_prefix(str.find(' ') + 1); //Удаляем "Bus " или "Stop "
	size_t begin = str.find_first_not_of(' ');
	size_t end = str.find_first_of(':', begin);
	if (end == std::string_view::npos || begin == end) {
		throw std::invalid_argument("Invalid stopname input");
	}
	auto name = str.substr(begin, end - begin);
	str.remove_prefix(end + 1);
	return name;
}

std::pair<std::string_view, int> input_processing::stop_parsing::ParseDistance(std::string_view str) {
	size_t new_end = str.find(' ');
	int distance = std::stoi(std::string(str.substr(0, new_end - 1)));
	size_t new_begin = str.find_first_not_of(' ', new_end);
	new_end = str.find(' ', new_begin);
	if (str.substr(new_begin, new_end - new_begin) == "to") {
		new_begin = str.find_first_not_of(' ', new_end);
	}
	return { str.substr(new_begin, str.size() - new_begin), distance };
}

std::vector<std::pair<std::string_view, int>> input_processing::stop_parsing::ParseDistances(std::string_view str ) {
	ParseName(str);
	ParseCoordinates(str); // удаляем координаты
	if (str.empty()) {
		return {};
	}
	std::vector<std::pair<std::string_view, int>> distance;
	size_t begin = str.find_first_not_of(' ');
	size_t end = str.find(',', begin);
	while (end != std::string_view::npos) {
		distance.push_back(ParseDistance(str.substr(begin, end - begin)));
		begin = str.find_first_not_of(' ', end + 1);
		end = str.find(',', begin);
	}
	distance.push_back(ParseDistance(str.substr(begin, str.size() - begin)));
	return distance;
}

Coordinates input_processing::stop_parsing::ParseCoordinates(std::string_view& str) {
	// находим широту
	size_t begin = str.find_first_not_of(' '); 
	size_t end = str.find(',', begin);
	double lat = std::stod(std::string(str.substr(begin, end - begin)));
	// нахоидм долготу
	begin = str.find_first_not_of(' ', end + 1);
	end = str.find_first_of(' ', begin);

	//определяем, есть ли за долготой пробелы и исходя из этого выбираем конец слова
	end = end == std::string_view::npos ? str.size() : end ;
	double lng = std::stod(std::string(str.substr(begin, end)));
	str.remove_prefix(end);
	return { lat, lng };
}

Stop input_processing::stop_parsing::ParseStop(std::string_view str) {
	Stop stop = { std::string(ParseName(str)), ParseCoordinates(str) };
	return stop;
}

Bus input_processing::bus_parsing::ParseBus(std::string_view str, input_processing::Instruction instruction, TransportCatalogue& catalouge) {

	std::string name = std::string(ParseName(str));
	Bus bus;
	bus.bus_name = std::move(name);
	if (instruction == input_processing::Instruction::BUS_RETURNING) {
		size_t start = str.find_first_not_of(' ');
		size_t end = str.find('-', start);
		while (end != std::string_view::npos) {
			auto sub = str.substr(start, end - start - 1);
			bus.stops_on_route.push_back(&catalouge.GetStop(sub));
			start = str.find_first_not_of(' ', end + str.find_first_not_of('-') + 1);
			end = str.find('-', start);
		}
		bus.stops_on_route.push_back(&catalouge.GetStop(str.substr(start, str.size())));
		bus.stops_on_route.reserve(bus.stops_on_route.size() * 2);

		for (auto it = bus.stops_on_route.rbegin() + 1; it != bus.stops_on_route.rend(); ++it) {
			bus.stops_on_route.push_back(*it);
		}
	}
	else {
		size_t start = str.find_first_not_of(' ');
		size_t end = str.find('>', start);
		while (end != std::string_view::npos) {
			auto sub = str.substr(start, end - start - 1);
			bus.stops_on_route.push_back(&catalouge.GetStop(sub));
			start = str.find_first_not_of(' ', end + str.find_first_not_of('>') + 1);
			end = str.find('>', start);
		}
		bus.stops_on_route.push_back(&catalouge.GetStop(str.substr(start, str.size())));
	}

	if (bus.stops_on_route[0] != bus.stops_on_route[bus.stops_on_route.size() - 1]) {
		throw std::invalid_argument("The first and last stops do not coincide");
	}

	return bus;
}


