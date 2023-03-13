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

	json_reader::JSONReader j_reader(catalogue_);

	json::Document doc = j_reader.JsonRead(cin);

	j_reader.FillTransportCatalogue(doc);

	renderer::Settings map_settings = j_reader.ReadWriteSettings(doc);

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

	j_reader.PrintAnswer(doc, rh, std::cout);


}


