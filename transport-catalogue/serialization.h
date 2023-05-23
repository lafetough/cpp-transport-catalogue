#pragma once

#include "transport_catalogue.h"
#include <transport_catalogue.pb.h>
#include <unordered_map>
#include <string_view>
#include <ostream>
#include "map_renderer.h"


class TransportSerializer {
public:
    TransportSerializer(const TransportCatalogue& c, const renderer::MapRenderer& map, const RoutingSettings& route)
        :catalogue_ref_(c), map_ref_(map), routing_settings_(route)
    {}

    void Serialize(std::ostream& out);


private:
    const TransportCatalogue& catalogue_ref_;
    const renderer::MapRenderer& map_ref_;
    const RoutingSettings& routing_settings_;
    serial::TransportCatalogue serialize_file;
    serial::StopList stop_list_;
    serial::BusList bus_list_;
    std::unordered_map<int, std::string_view> id_to_stopname_;
    std::unordered_map<std::string_view, int> stopname_to_id;


    void ParseStop();
    void ParseBus();
    void ParseStopToDist();
    void ParseRenderSettings();
    void ParseRoutingSettings();


    serial::StopList::Stop StopToSerialStop(const Stop& stop, int id);
    serial::BusList::Bus BusToSerialBus(const Bus& bus, int id);
    serial::StopsToDist StopsDistToSerial(const std::pair<const Stop*, const Stop*>& stop_pair, double dist);
    serial::Color ParseColor(const svg::Color& color);
};

struct DeserializedContent {
    TransportCatalogue catalogue;
    renderer::MapRenderer map;
    RoutingSettings routing_settings;
};


class TransportDeserializer {
public:

    using StopPairsToDistElem = std::pair<std::pair<const Stop*, const Stop*>, double>;
    DeserializedContent Deserialize(std::istream& in);


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

    svg::Color ColorCheck(serial::Color serial_color) const;

};









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





