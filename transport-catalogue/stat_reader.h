#pragma once
#include <ostream>
#include "transport_catalogue.h"


namespace output_processing {

	enum class Instruction
	{
		STOP,
		BUS
	};

	void ExecuteRequests(const int num, TransportCatalogue& catalouge);

	Instruction ParseInstructionType(std::string_view str);

	std::string ParseName(std::string_view str);

	namespace bus {
		std::ostream& PrintInformation(std::ostream& os, const Bus& bus, TransportCatalogue& catalouge);
	}
	namespace stop {
		std::ostream& PrintInformation(std::ostream& os, std::string_view& stop, TransportCatalogue& catalouge);
	}
}