#include "transport_router.h"
#include <iostream>


TransportRouter::TransportRouter(RoutingSettings settings, const TransportCatalogue& c)
	:settigns_(std::move(settings)), 
	catalogue_(c), 
	graph_(c.GetAllStops().size() * 2), 
	router_(FillGraph())
{}

const graph::DirectedWeightedGraph<double>& TransportRouter::FillGraph() {
	MakeWaitEdges();

	const std::deque<Bus>& buses = catalogue_.GetAllBuses();

	for (const Bus& bus : buses) {

		if (!bus.is_rounded) {
			MakeRoundingBusEdges(bus);
		}
		else {
			MakeNonRoundingBusEdges(bus);
		}
	}

	return graph_;
}

void TransportRouter::MakeWaitEdges() {
	graph::VertexId	curr_free_id = 0;

	const std::deque<Stop>& stops = catalogue_.GetAllStops();
	for (const Stop& stop : stops) {
		StopGraphContain stop_graph = { stop, curr_free_id++, curr_free_id++ };
		auto id = graph_.AddEdge({ stop_graph.start_waiting, stop_graph.end_waiting, settigns_.wait_time, true });
		//std::cout << id << " Stop waiting: "s << stop.stop_name << std::endl;

		edge_info_by_edge_id_[id] = WaitEdge{ stop.stop_name, settigns_.wait_time };

		//edge_info_by_edge_id_[id] = std::move(w);
		stops_vertex_cont_.insert({ stop.stop_name, std::move(stop_graph) });
	}
}

void TransportRouter::MakeRoundingBusEdges(const Bus& bus) {
	//перебираем остановки отправления
	for (int stop_from_index = 0; stop_from_index < bus.stops_on_route.size() / 2 + 1; ++stop_from_index) {
		const Stop* stop_from = bus.stops_on_route[stop_from_index];
		//перепираем остановки прибытия
		for (int stop_to_index = 0; stop_to_index < bus.stops_on_route.size() / 2 + 1; ++stop_to_index) {
			const Stop* stop_to = bus.stops_on_route[stop_to_index];
			//если попали на одну и ту же остановку - пропускаем
			if (stop_from_index == stop_to_index) {
				continue;
			}
			// берем информацию о вершинах остановок
			const auto stop_from_graph = stops_vertex_cont_.at(stop_from->stop_name);
			const auto stop_to_graph = stops_vertex_cont_.at(stop_to->stop_name);
			int distance_between_stops = 0;

			if (stop_from_index < stop_to_index) {
				for (int i = stop_from_index; i < stop_to_index; ++i) {

					const Stop* from = bus.stops_on_route.at(i);
					const Stop* to = bus.stops_on_route.at(i + 1);

					distance_between_stops += catalogue_.GetDistance({ from, to });
				}
			}
			else {
				for (int i = stop_from_index; i > stop_to_index; --i) {

					const Stop* from = bus.stops_on_route.at(i);
					const Stop* to = bus.stops_on_route.at(i - 1);

					distance_between_stops += catalogue_.GetDistance({ from, to });
				}
			}
			double time = (distance_between_stops * 1.0 / 1000) / settigns_.speed * 60;
			//добовляем ребро

			auto id = graph_.AddEdge({ stop_from_graph.end_waiting, stop_to_graph.start_waiting, time });

			int span_count = std::abs(stop_from_index - stop_to_index);

			edge_info_by_edge_id_[id] = BusEdge{ bus.bus_name, span_count, time };
			//std::cout << id << " From: "s << stop_from_graph.stop.stop_name << " To: "s << stop_to_graph.stop.stop_name << ", time: "s << time << " distance: "s << distance_between_stops << std::endl;

		}
	}
}

void TransportRouter::MakeNonRoundingBusEdges(const Bus& bus) {
	//перепираем остановки отправления
	for (int stop_from_index = 0; stop_from_index < bus.stops_on_route.size() - 1; ++stop_from_index) {
		const Stop* stop_from = bus.stops_on_route[stop_from_index];
		//перебираем остановки прибытия
		for (int stop_to_index = stop_from_index + 1; stop_to_index < bus.stops_on_route.size(); ++stop_to_index) {
			const Stop* stop_to = bus.stops_on_route[stop_to_index];
			//если наткнулися на ту одинаковые остановки (нужно, чтобы не считать расстояние от конечной остановки до самой себя же)
			if (stop_to == stop_from) {
				continue;
			}
			// берем информацию о вершинах остановок
			const auto stop_from_graph = stops_vertex_cont_.at(stop_from->stop_name);
			const auto stop_to_graph = stops_vertex_cont_.at(stop_to->stop_name);

			int distance_between_stops = 0;

			for (int i = stop_from_index; i < stop_to_index; ++i) {

				const Stop* from = bus.stops_on_route.at(i);
				const Stop* to = bus.stops_on_route.at(i + 1);

				distance_between_stops += catalogue_.GetDistance({ from, to });
			}
			double time = (distance_between_stops * 1.0 / 1000) / settigns_.speed * 60;


			//добовляем ребро
			auto id = graph_.AddEdge({ stop_from_graph.end_waiting, stop_to_graph.start_waiting, time });

			int span_count = std::abs(stop_from_index - stop_to_index);

			edge_info_by_edge_id_[id] = BusEdge{ bus.bus_name, span_count, time };
			//std::cout << id << " From: "s << stop_from_graph.stop.stop_name << " To: "s << stop_to_graph.stop.stop_name << ", time: "s << time << " distance: "s << distance_between_stops << std::endl;

		}

	}
}



const std::unordered_map<std::string_view, StopGraphContain>& TransportRouter::GetVertexes() const {
	return stops_vertex_cont_;
}

const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
	return graph_;
}

const StopGraphContain& TransportRouter::GetStopVertexInfo(const std::string_view stop_name) const {
	return stops_vertex_cont_.at(stop_name);
}

const std::optional<graph::Router<double>::RouteInfo> TransportRouter::Build(graph::VertexId from, graph::VertexId to) const {
	return router_.BuildRoute(from, to);
}

const EdgeInfo& TransportRouter::GetEdgeInfoByEdgeId(const graph::EdgeId id) const {
	return edge_info_by_edge_id_.at(id);
}


