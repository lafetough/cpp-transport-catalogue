#pragma once
#include <string>
#include <string_view>
#include <stdexcept>
#include "transport_catalogue.h"
#include <istream>

namespace input_processing {

	namespace thread_reading {

		std::string ReadLine(std::istream& in);

		int ReadLineWithNumber(std::istream& in);

	}

	void FillTransportCatalogue(const std::vector<std::string>& requests, TransportCatalogue& catalouge);

	enum class Instruction
	{
		STOP,
		BUS_CIRCLE,
		BUS_RETURNING,
	};

	Instruction ParseInstructionType(std::string_view str);

	std::string_view ParseName(std::string_view& str);

	namespace stop_parsing {

		Coordinates ParseCoordinates(std::string_view& str);

		Stop ParseStop(std::string_view str);

		std::vector<std::pair<std::string_view, int>> ParseDistances(std::string_view str);

		std::pair<std::string_view, int> ParseDistance(std::string_view str);
	}

	namespace bus_parsing {

		Bus ParseBus(std::string_view str, Instruction instruction, TransportCatalogue& catalouge);

		RouteLengthInformation ComputeRouteLength(const Bus& bus, TransportCatalogue& catalouge);
	}

}