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
#include "transport_router.h"

using namespace std;

std::deque<Stop> MakeOnlyUsedStops(const TransportCatalogue& catalogue_) {
	std::deque<Stop> used_stops;

	for (const Stop& stop : catalogue_.GetAllStops()) {
		if (catalogue_.GetBusesByStop(stop.stop_name).empty()) {
			continue;
		}
		used_stops.push_back(stop);
	}
	return used_stops;
}


//
//void Test() {
//	{
//		TransportCatalogue catalogue_;
//		json_reader::JSONReader j_reader(catalogue_);
//
//		std::ifstream in("C:\\Transport2\\cpp-transport-catalogue\\transport-catalogue\\input.json"s);
//
//		json::Document doc = j_reader.JsonRead(in);
//		j_reader.FillTransportCatalogue(doc);
//
//		TransportRouter tr({ 30, 2 }, catalogue_);
//		tr.FillGraph();
//		const auto& a = tr.GetGraph();
//		graph::Router<double> r(a);
//
//		const auto vertexes = tr.GetVertexes();
//		{
//			const auto route = r.BuildRoute(vertexes.at("Biryulyovo Zapadnoye"s).start_waiting, vertexes.at("Apteka"s).start_waiting);
//
//			assert(route->weight == 7.42 && route->edges.size() == 4);
//		}
//		{
//			const auto route = r.BuildRoute(vertexes.at("Biryulyovo Zapadnoye"s).start_waiting, vertexes.at("Pokrovskaya"s).start_waiting);
//
//			assert(route->weight == 11.44 && (route->edges.size() == 2));
//		}
//		{
//			const auto route = r.BuildRoute(vertexes.at("Biryulyovo Tovarnaya"s).start_waiting, vertexes.at("Pokrovskaya"s).start_waiting);
//
//			assert(route->weight == 10.7 && route->edges.size() == 4);
//		}
//		{
//			const auto route = r.BuildRoute(vertexes.at("Biryulyovo Tovarnaya"s).start_waiting, vertexes.at("Biryulyovo Zapadnoye"s).start_waiting);
//
//			//assert(route->weight == 8.5599999987 && route->edges.size() == 2);
//		}
//		{
//			const auto route = r.BuildRoute(vertexes.at("Biryulyovo Tovarnaya"s).start_waiting, vertexes.at("Prazhskaya"s).start_waiting);
//
//			assert(route->weight == 16.32 && route->edges.size() == 2);
//		}
//		{
//			const auto route = r.BuildRoute(vertexes.at("Apteka"s).start_waiting, vertexes.at("Biryulyovo Tovarnaya"s).start_waiting);
//
//			assert(route->weight == 12.04 && route->edges.size() == 4);
//		}
//		{
//			const auto route = r.BuildRoute(vertexes.at("Biryulyovo Zapadnoye"s).start_waiting, vertexes.at("Tolstopaltsevo"s).start_waiting);
//
//			//assert(route->weight == 12.04 && route->edges.size() == 4);
//		}
//
//
//	}
//}
//
//void Test2() {
//	TransportCatalogue catalogue_;
//	json_reader::JSONReader j_reader(catalogue_);
//
//	std::ifstream in("C:\\Transport2\\cpp-transport-catalogue\\transport-catalogue\\input.json"s);
//
//	json::Document doc = j_reader.JsonRead(in);
//	j_reader.FillTransportCatalogue(doc);
//
//	TransportRouter tr({ 30, 2 }, catalogue_);
//	tr.FillGraph();
//	const auto& a = tr.GetGraph();
//	graph::Router<double> r(a);
//
//	const auto vertexes = tr.GetVertexes();
//
//	{
//		const auto route = r.BuildRoute(vertexes.at("Zagorye"s).start_waiting, vertexes.at("Moskvorechye"s).start_waiting);
//
//		assert(route->weight == 29.26 && route->edges.size() == 4);
//	}
//	{
//		const auto route = r.BuildRoute(vertexes.at("Moskvorechye"s).start_waiting, vertexes.at("Zagorye"s).start_waiting);
//
//		assert(route->weight == 22 && route->edges.size() == 2);
//	}
//	{
//		const auto route = r.BuildRoute(vertexes.at("Lipetskaya ulitsa 40"s).start_waiting, vertexes.at("Lipetskaya ulitsa 40"s).start_waiting);
//
//		assert(route->weight == 0 && route->edges.size() == 0);
//	}
//
//}
//

int main() {

	
	TransportCatalogue catalogue_;

	json_reader::JSONReader j_reader(catalogue_);

	std::ifstream in("C:\\Transport2\\cpp-transport-catalogue\\transport-catalogue\\input.json"s);

	json::Document doc = j_reader.JsonRead(std::cin);

	j_reader.FillTransportCatalogue(doc);

	renderer::Settings map_settings = j_reader.ReadWriteSettings(doc);

	std::deque<Stop> used_stops = MakeOnlyUsedStops(catalogue_);

	renderer::SphereProjector projector(
		used_stops.begin(),
		used_stops.end(),
		map_settings.width,
		map_settings.height,
		map_settings.padding
	);

	renderer::MapRenderer mr(map_settings, projector);
	TransportRouter tr(j_reader.ReadRoutingSettings(doc), catalogue_);

	RequestHandler rh(catalogue_, mr, tr);

	//ofstream ofs("C:\\Transport2\\cpp-transport-catalogue\\transport-catalogue\\output.json"s);

	j_reader.PrintAnswer(doc, rh, std::cout);


}