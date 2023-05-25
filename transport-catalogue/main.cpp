#include <iostream>
#include "json_reader.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include <vector>
#include <cassert>
#include <unordered_set>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <windows.h>
using namespace std;

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

        //std::ifstream json_in("C:/Users/User/Downloads/s14_part3_opentest/s14_3_opentest_1_make_base.json"s);

        //std::ifstream json_in("../transport-catalogue/input_mb.json"s);

        TransportCatalogue catalogue;
        json_reader::JSONHandler j_handler(json_reader::JsonRead(std::cin), &catalogue);
        SerializingSettings serial_settings = json_reader::ParseSerializeSettings(j_handler.GetJsonDoc());

        j_handler.FillTransportCatalogue();

        renderer::Settings map_settings = j_handler.ParseRenderSettings();
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




        TransportSerializer serializer(catalogue, map, routing_settings, router);

        std::ofstream out_file(serial_settings.file_name, ios::binary);
        serializer.Serialize(out_file);

    } else if (mode == "process_requests"sv) {

        //std::ifstream json_in("C:/Users/User/Downloads/s14_part3_opentest/s14_3_opentest_1_process_requests.json");

        //std::ifstream json_in("../transport-catalogue/input_pr.json"s);

        json::Document json_doc = json_reader::JsonRead(std::cin);

        SerializingSettings serial_settings = json_reader::ParseSerializeSettings(json_doc);
        std::ifstream in_file(serial_settings.file_name, ios::binary);

        TransportDeserializer deserializer(in_file);

        TransportCatalogue catalogue = deserializer.DeserializeCatalogue();
        renderer::MapRenderer map = deserializer.DeserializeMap(catalogue);
        TransportRouter transport_router = deserializer.DeserializeRouter(catalogue);

        json_reader::JSONHandler j_handler(json_doc, &catalogue);

        RequestHandler handler(&catalogue, &map, &transport_router);
        //std::ofstream out_json("output.json"s);
        j_handler.PrintAnswer(handler, std::cout);


    } else {
        PrintUsage();
        return 1;
    }
}
