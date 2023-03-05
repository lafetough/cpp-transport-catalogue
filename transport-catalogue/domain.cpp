#include "domain.h"


bool Stop::operator==(const Stop& other) const {
	return stop_name == other.stop_name && cooradinates == other.cooradinates;
}

bool Stop::empty() const {
	return stop_name.empty();
}

std::size_t StopPairHasher::operator() (const std::pair<Stop*, Stop*> pr1) const {
	size_t h1 = hasher(pr1.first->cooradinates.lat * N);
	size_t h2 = hasher(pr1.first->cooradinates.lng * (N * N));
	size_t h3 = hasher(pr1.second->cooradinates.lat * (N * N * N));
	size_t h4 = hasher(pr1.second->cooradinates.lng * (N * N * N * N));
	return h1 + h2 + h3 + h4;
}

bool Bus::operator==(const Bus& other) const {
	return bus_name == other.bus_name && stops_on_route == other.stops_on_route;
}




/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области
 * (domain) вашего приложения и не зависят от транспортного справочника. Например Автобусные
 * маршруты и Остановки.
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */