#include <iostream>
#include "transport_catalogue.h"
#include <vector>
#include <cassert>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <execution>
#include <sstream>
#include "json_reader.h"
#include <iomanip>
#include "map_renderer.h"
#include "request_handler.h"


using namespace std;


int main() {
	
	TransportCatalogue catalogue_;

	//tests::Start();

	//std::ifstream file("C:\\Users\\NyaRoar\\source\\repos\\Project\\Project\\input.json"s);



	json::Document doc = json::Load(std::cin);

	json_reader::FillTransportCatalogue(doc, catalogue_);

	renderer::Settings map_settings = json_reader::ReadSettings(doc);

	std::deque<Stop> used_stops;

	for (const Stop& stop : catalogue_.GetAllStops()) {
		if (catalogue_.GetBusesByStop(stop.stop_name).empty()) {
			continue;
		}
		used_stops.push_back(stop);
	}

	renderer::SphereProjector projector(
		used_stops.begin(),
		used_stops.end(),
		map_settings.width,
		map_settings.height,
		map_settings.padding
	);

	renderer::MapRenderer mr(map_settings, projector);

	RequestHandler rh(catalogue_, mr);

	json_reader::PrintAnswer(doc, rh, std::cout);


}


