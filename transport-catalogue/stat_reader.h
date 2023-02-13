#pragma once
#include <ostream>
#include "transport_catalogue.h"


namespace output_processing {

	enum class Instruction
	{
		STOP,
		BUS
	};

	Instruction Identifaer(std::string_view str);

	std::string NameParsing(std::string_view str);

	namespace bus {
		std::ostream& Information(std::ostream& os, const Bus& bus, TransportCatalogue& catalouge);
	}
	namespace stop {
		std::ostream& Information(std::ostream& os, std::string_view& stop, TransportCatalogue& catalouge);
	}
}