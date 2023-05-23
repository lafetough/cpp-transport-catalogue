#include <iostream>
#include "json_reader.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include <vector>
#include <cassert>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <execution>
#include <sstream>
#include <iomanip>
#include <windows.h>
using namespace std;


//int main() {

	
//	TransportCatalogue catalogue_;

//	json_reader::JSONReader j_reader(catalogue_);

//	std::ifstream in("C:\\Transport2\\cpp-transport-catalogue\\transport-catalogue\\input.json"s);

//	json::Document doc = j_reader.JsonRead(std::cin);

//	j_reader.FillTransportCatalogue(doc);

//	renderer::Settings map_settings = j_reader.ReadWriteSettings(doc);

//	std::deque<Stop> used_stops = MakeOnlyUsedStops(catalogue_);

//	renderer::SphereProjector projector(
//		used_stops.begin(),
//		used_stops.end(),
//		map_settings.width,
//		map_settings.height,
//		map_settings.padding
//	);

//	renderer::MapRenderer mr(map_settings, projector);
//	TransportRouter tr(j_reader.ReadRoutingSettings(doc), catalogue_);

//	RequestHandler rh(catalogue_, mr, tr);

//	//ofstream ofs("C:\\Transport2\\cpp-transport-catalogue\\transport-catalogue\\output.json"s);

//	j_reader.PrintAnswer(doc, rh, std::cout);


//}

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);


    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {

        std::ifstream json_in("C:/Users/User/Downloads/s14_part2_opentest/s14_2_opentest_3_make_base.json"s);

        //std::ifstream json_in("../transport-catalogue/input_mb.json"s);

        TransportCatalogue catalogue;
        json_reader::JSONHandler j_handler(json_reader::JsonRead(json_in), &catalogue);
        SerializingSettings serial_settings = json_reader::ParseSerializeSettings(j_handler.GetJsonDoc());

        j_handler.FillTransportCatalogue();

        renderer::Settings map_settings = j_handler.ReadWriteSettings();
        std::deque<Stop> only_used_stops = MakeOnlyUsedStops(catalogue);
        renderer::SphereProjector projector(
            only_used_stops.begin(),
            only_used_stops.end(),
            map_settings.width,
            map_settings.height,
            map_settings.padding);
        renderer::MapRenderer map(map_settings, projector);

        RoutingSettings routing_settings = j_handler.ReadRoutingSettings();

        TransportRouter router(routing_settings, catalogue);

        TransportSerializer serializer(catalogue, map, routing_settings);

        std::ofstream out_file(serial_settings.file_name, ios::binary);
        serializer.Serialize(out_file);

    } else if (mode == "process_requests"sv) {

        std::ifstream json_in("C:/Users/User/Downloads/s14_part2_opentest/s14_2_opentest_3_process_requests.json");

        //std::ifstream json_in("../transport-catalogue/input_pr.json"s);
        TransportDeserializer deserializer;

        json::Document json_doc = json_reader::JsonRead(json_in);

        SerializingSettings serial_settings = json_reader::ParseSerializeSettings(json_doc);
        std::ifstream in_file(serial_settings.file_name, ios::binary);

        DeserializedContent content = deserializer.Deserialize(in_file);

        json_reader::JSONHandler j_handler(json_doc, &content.catalogue );

        RequestHandler handler(&content.catalogue, &content.map, nullptr);
        std::ofstream out_json("output.json"s);
        j_handler.PrintAnswer(handler, out_json);


    } else {
        PrintUsage();
        return 1;
    }
}
