#pragma once

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"
#include <variant>

using EdgeInfo = std::variant<std::monostate, WaitEdge, BusEdge>;

class TransportRouter {
public:

	TransportRouter(RoutingSettings settings, const TransportCatalogue& c);
	
	const std::optional<graph::Router<double>::RouteInfo> Build(graph::VertexId from, graph::VertexId to) const;

	const std::unordered_map<std::string_view, StopGraphContain>& GetVertexes() const;

	const graph::DirectedWeightedGraph<double>& GetGraph() const;

	const StopGraphContain& GetStopVertexInfo(const std::string_view stop_name) const;

	const EdgeInfo& GetEdgeInfoByEdgeId(const graph::EdgeId id) const; 


private:
	const RoutingSettings settigns_;
	const TransportCatalogue& catalogue_;
	graph::DirectedWeightedGraph<double> graph_;
	std::unordered_map<graph::EdgeId, EdgeInfo> edge_info_by_edge_id_;
	std::unordered_map<std::string_view, StopGraphContain> stops_vertex_cont_;
	const graph::Router<double> router_;



	const graph::DirectedWeightedGraph<double>& FillGraph();

	void MakeWaitEdges();

	void MakeRoundingBusEdges(const Bus& bus);

	void MakeNonRoundingBusEdges(const Bus& bus); 
};