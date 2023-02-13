#pragma once
#include <string>
#include <string_view>
#include <stdexcept>
#include "transport_catalogue.h"

namespace input_processing {

	namespace thread_reading {

		std::string ReadLine();

		int ReadLineWithNumber();

	}
	enum class Instruction
	{
		STOP,
		BUS_CIRCLE,
		BUS_RETURNING,
	};

	Instruction Identifaer(std::string_view str);

	std::string_view NameParcing(std::string_view& str);

	namespace stop_parsing {

		Coordinates CoordinatesParsing(std::string_view& str);

		Stop StopParsing(std::string_view str);

		std::vector<std::pair<std::string_view, int>> DistancesParsing(std::string_view str);

		std::pair<std::string_view, int> ParticalDistanceParcing(std::string_view str);
	}

	namespace bus_parsing {

		Bus BusParsing(std::string_view str, Instruction instruction, TransportCatalogue& catalouge);

	}

}