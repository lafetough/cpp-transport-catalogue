#pragma once

#include "transport_catalogue.h"
#include <transport_catalogue.pb.h>
#include <transport_router.pb.h>
#include "transport_router.h"
#include <unordered_map>
#include <string_view>
#include <ostream>
#include "map_renderer.h"


class TransportSerializer {
public:
    TransportSerializer(const TransportCatalogue& c, const renderer::MapRenderer& map, const RoutingSettings& route, const TransportRouter& router)
        :catalogue_ref_(c), map_ref_(map), routing_settings_(route), router_(router)
    {}

    void Serialize(std::ostream& out);


private:
    const TransportCatalogue& catalogue_ref_;
    const renderer::MapRenderer& map_ref_;
    const RoutingSettings& routing_settings_;
    const TransportRouter& router_;

    serial::TransportCatalogue serialize_file;
    serial::StopList stop_list_;
    serial::BusList bus_list_;
    std::unordered_map<int, std::string_view> id_to_stopname_;
    std::unordered_map<std::string_view, int> stopname_to_id;


    void ParseStop();
    void ParseBus();
    void ParseStopToDist();
    void ParseRenderSettings();

    serial::RoutingSettings ParseRoutingSettings();
    void ParseTR();
    serial::Router ParseRouter();
    serial::Graph ParseGraph();

    void InsertEdgesInfo(graph::EdgeId edge_id);


    serial::StopList::Stop StopToSerialStop(const Stop& stop, int id);
    serial::BusList::Bus BusToSerialBus(const Bus& bus, int id);
    serial::StopsToDist StopsDistToSerial(const std::pair<const Stop*, const Stop*>& stop_pair, double dist);
    serial::Color ParseColor(const svg::Color& color);
};

struct DeserializedContent {
    std::unique_ptr<TransportCatalogue> catalogue;
    std::unique_ptr<renderer::MapRenderer> map;
    std::unique_ptr<TransportRouter> transport_router;
};


class TransportDeserializer {
public:

    TransportDeserializer(std::istream& serial_in)
    {
        serial_.ParseFromIstream(&serial_in);
    }

    using StopPairsToDistElem = std::pair<std::pair<const Stop*, const Stop*>, double>;
    DeserializedContent Deserialize();

    TransportCatalogue DeserializeCatalogue();
    renderer::MapRenderer DeserializeMap(const TransportCatalogue& c);
    TransportRouter DeserializeRouter(const TransportCatalogue& c);

    Stop SerialStopToStop(const serial::StopList::Stop& stop) const;
    Bus SerialBusToBus(const serial::BusList::Bus& bus, const TransportCatalogue& c) const;
    std::pair<std::pair<std::string_view, std::string_view>, double> NormalizeSerialStopToDist(
        const serial::StopsToDist& stops_to_dist,
        const TransportCatalogue& c) const ;

private:
    serial::TransportCatalogue serial_;
    std::unordered_map<int, std::string_view> id_to_stopname_;



    TransportCatalogue ConvertTC();
    renderer::MapRenderer ConvertMap(const TransportCatalogue& catalogue, const renderer::Settings& map_settings) const;
    renderer::Settings ConvertMapSettings() const;
    RoutingSettings ConvertRoutingSettings() const;
    graph::Router<double> ConvertRouter(const graph::DirectedWeightedGraph<double>& graph_ref) const;
    graph::DirectedWeightedGraph<double> ConvertGraph() const;
    TransportRouter ConvertTransportRouter(const TransportCatalogue& catalogue) const;
    graph::Router<double>::RoutesInternalData ConvertIternalData() const;

    svg::Color ColorCheck(serial::Color serial_color) const;

};

template<typename Weight>
graph::Edge<Weight> SerialEdgeToNormal(const serial::Edge& edge_serial);







//class Serializator {
//public:
//    Serializator(json_reader::JSONSerializer& s)
//        :json_ser_(&s)
//    {}

//    void Serialize(std::ostream& out);

//private:
//    json_reader::JSONSerializer* json_ser_ = nullptr;
//    serial::TransportCatalogue serial_catalogue_ ;
//};


//class Deserializator {
//public:

//    TransportCatalogue Deserialize(std::istream& in);

//private:
//    serial::TransportCatalogue serial_catalogue_;
//};





