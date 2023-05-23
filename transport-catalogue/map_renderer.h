#pragma once
#include "svg.h"
#include "geo.h"
#include "transport_catalogue.h"
#include <algorithm>
#include "domain.h"
#include <set>
#include <cmath>

namespace renderer {

	inline const double EPSILON = 1e-6;
	bool IsZero(double value);

	class SphereProjector {
	public:
		// points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
		template <typename PointInputIt>
		SphereProjector(PointInputIt points_begin, PointInputIt points_end,
			double max_width, double max_height, double padding)
			: padding_(padding) //
		{
			// Если точки поверхности сферы не заданы, вычислять нечего
			if (points_begin == points_end) {
				return;
			}

			// Находим точки с минимальной и максимальной долготой
			const auto [left_it, right_it] = std::minmax_element(
				points_begin, points_end,
				[](auto lhs, auto rhs) { return lhs.cooradinates.lng < rhs.cooradinates.lng; });
			min_lon_ = left_it->cooradinates.lng;
			const double max_lon = right_it->cooradinates.lng;

			// Находим точки с минимальной и максимальной широтой
			const auto [bottom_it, top_it] = std::minmax_element(
				points_begin, points_end,
				[](auto lhs, auto rhs) { return lhs.cooradinates.lat < rhs.cooradinates.lat; });
			const double min_lat = bottom_it->cooradinates.lat;
			max_lat_ = top_it->cooradinates.lat;

			// Вычисляем коэффициент масштабирования вдоль координаты x
            std::optional<double> width_zoom;
            if (!IsZero(max_lon - min_lon_)) {
                width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
            }

			// Вычисляем коэффициент масштабирования вдоль координаты y
			std::optional<double> height_zoom;
			if (!IsZero(max_lat_ - min_lat)) {
				height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
			}

			if (width_zoom && height_zoom) {
				// Коэффициенты масштабирования по ширине и высоте ненулевые,
				// берём минимальный из них
				zoom_coeff_ = std::min(*width_zoom, *height_zoom);
			}
			else if (width_zoom) {
				// Коэффициент масштабирования по ширине ненулевой, используем его
				zoom_coeff_ = *width_zoom;
			}
			else if (height_zoom) {
				// Коэффициент масштабирования по высоте ненулевой, используем его
				zoom_coeff_ = *height_zoom;
			}
		}

		// Проецирует широту и долготу в координаты внутри SVG-изображения
		svg::Point operator()(geo::Coordinates coords) const;

	private:
		double padding_;
		double min_lon_ = 0;
		double max_lat_ = 0;
		double zoom_coeff_ = 0;
	};


	struct Settings
	{

		Settings() = default;

		using offset = svg::Point;

		using palette = std::vector<svg::Color>;

		double width;
		double height;

		double padding;

		double line_width;
		double stop_radius;

		int bus_label_font_size;
		offset bus_label_offset;

		int stop_label_font_size;
		offset stop_label_offset;

		svg::Color underlayer_color;
		double underlayer_width;

		palette color_palette;

	};

	class MapRenderer {
	public:

        MapRenderer(const Settings& settings, const SphereProjector& projector);

		svg::Document RenderObj(const TransportCatalogue& catalogue_);

        const Settings& GetSettings() const;
        const SphereProjector& GetProjector() const;


	private:
		const Settings settings_;
		const SphereProjector projector_;
		int palette_color_num = 0;

		void AddText(const Bus* bus, svg::Document& doc);
		void AddText(const Stop* stop, svg::Document& doc);

		template<typename SortedArr>
		void AddStopsOnRoute(SortedArr stops, svg::Document& doc);

	};

	template<typename SortedArr>
	void MapRenderer::AddStopsOnRoute(SortedArr stops, svg::Document& doc) {

		for (const Stop* stop : stops) {
			std::unique_ptr<svg::Circle> stop_label = std::make_unique<svg::Circle>();
			stop_label->SetCenter(projector_(stop->cooradinates))
				.SetRadius(settings_.stop_radius)
				.SetFillColor("white"s);
			doc.AddPtr(std::move(stop_label));
		}

	}

}
