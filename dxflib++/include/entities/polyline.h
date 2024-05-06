#pragma once
#include "lwpolyline.h"
#include "mathlib.h"
#include "entity.h"
#include <vector>

namespace dxflib::entities
{
	/**
	 * \brief AutoCAD LT 2013 group codes for the lwpoyline entity
	 */
	namespace group_codes
	{
		enum class polyline
		{
			vertex_count = 90,
			polyline_flag = 70,
			elevation = 38,
			x_value = 10,
			y_value = 20,
			vertex_id = 91,
			bulge = 42,
			starting_width = 40,
			ending_width = 41,
			width = 43,
		};
	}

	struct polyline_buffer : virtual entity_buffer_base
	{
		// Geometric Properties
		std::vector<double> x_values;
		std::vector<double> y_values;
		std::vector<double> bulge_values;
		// Other Properties
		bool polyline_flag{};
		double elevation{};
		int vertex_count{};
		double starting_width{};
		double ending_width{};
		double width{};


		// Parse function override
		int parse(const std::string& cl, const std::string& nl) override;
		void free() override;
	};

	class line;

	// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
	class polyline : public entity
	{
	public:
		// Constructors
		explicit polyline(polyline_buffer&);
		explicit polyline(const line& l);
		// Public Interface

		// Get
		int get_vertex_count() const { return vertex_count_; } // Returns the Vertex Count
		bool is_closed() const { return is_closed_; } // Returns True if the polyline is closed
		double get_elevation() const { return elevation_; } // Returns the elevation of the polyline
		double get_starting_width() const { return starting_width_; } // Returns the starting width of the polyline
		double get_ending_width() const { return ending_width_; } // Returns the ending width of the polyline
		double get_width() const { return width_; } // Returns the Global Width of the polyline
		const std::vector<geoline>& get_lines() const { return lines_; }
		// Returns the geolines that the polyline is made from
		double get_length() const { return length_; } // Returns the length of the polyline
		double get_area() const { return area_; } // Returns the area of the polyline
		double is_drawn_ccw() const { return drawn_counter_clockwise_; }
		const polyline* get_within_pointer() const { return within_pointer_; }
		// Set
		void set_elevation(const double new_elevation) // Sets the elevation of the polyline
		{
			elevation_ = new_elevation;
		}

		void move_vertex(int id, const vertex& new_vertex); // Moves the vertex[id] to new location

		// Other functions
		bool within(const polyline& pl);
		friend std::ostream& operator<<(std::ostream& os, polyline);

	private:
		// Properties
		int vertex_count_; // Total number of verticies in the polyline
		bool is_closed_; // returns true if the polyline is closed
		double elevation_; // elevation of the polyline
		double starting_width_; // the starting global width 
		double ending_width_; // the ending global width
		double width_; // the global width: only if starting width and ending width are 0
		std::vector<geoline> lines_; // the component lines of the polyline
		double length_{}; // total length of the polyline
		double area_{}; // Total area of the polyline
		bool drawn_counter_clockwise_{};

		const polyline* within_pointer_{ nullptr };

		/**
		 * \brief Function that calculates the total length & area of the polyline
		 */
		void calc_geometry();
	};
}
