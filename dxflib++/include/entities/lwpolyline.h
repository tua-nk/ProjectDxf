#pragma once
#include "entities/point.h"
#include "mathlib.h"
#include "entity.h"
#include <vector>

namespace dxflib::entities
{
	/**
	 * \brief Geometric line segment
	 */
	class geoline
	{
	public:
		static constexpr int bulge_null{-2};
		/**
		 * \brief Geometric line segment to be used as base for lwpolyline
		 * \param v0 start vertex
		 * \param v1 end vertex
		 * \param bulge bulge value if arc segment
		 */
		explicit geoline(const vertex& v0, const vertex& v1, double bulge = bulge_null);
		static std::vector<geoline> geoline_binder(const std::vector<double>& x,
		                                           const std::vector<double>& y, const std::vector<double>& bulge,
		                                           bool is_closed);

		// Returns the draw direction of a set of geolines
		static double draw_direction(const geoline&, const geoline&);

		// Public Interface 
		const vertex& operator[](int id) const; // Returns a vertex of the geoline 0 or 1
		vertex& operator[](int id); // Returns a vertex of the geoline 0 or 1
		inline double get_length() const; // Returns the length of the geoline
		double get_bulge() const { return bulge_; } // Returns the bulge of the geoline
		double get_area() const { return area_; } // Returns the area between the geoline and the x-axis
		inline double get_radius() const; // Returns the radius of the geoline: INF if bulge == bulge_null
		double get_angle() const; // Returns the angle of the geoline: INF if bulge == bulge_null
		friend std::ostream& operator<<(std::ostream& os, geoline& geoline);

	protected:
		// Properties and members
		vertex v0_;
		vertex v1_;
		double bulge_;
		double length_;
		double radius_;
		double total_angle_;
		double area_;
	};

	/**
	 * \brief AutoCAD LT 2013 group codes for the lwpoyline entity
	 */
	namespace group_codes
	{
		enum class lwpolyline
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

	struct lwpolyline_buffer : virtual entity_buffer_base
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
	class lwpolyline : public entity
	{
	public:
		// Constructors
		explicit lwpolyline(lwpolyline_buffer&);
		explicit lwpolyline(const line& l);
		// Public Interface

		// Get
		int get_vertex_count() const { return vertex_count_; } // Returns the Vertex Count
		bool is_closed() const { return is_closed_; } // Returns True if the lwpolyline is closed
		double get_elevation() const { return elevation_; } // Returns the elevation of the lwpolyline
		double get_starting_width() const { return starting_width_; } // Returns the starting width of the lwpolyline
		double get_ending_width() const { return ending_width_; } // Returns the ending width of the lwpolyline
		double get_width() const { return width_; } // Returns the Global Width of the lwpolyline
		const std::vector<geoline>& get_lines() const { return lines_; }
		// Returns the geolines that the lwpolyline is made from
		double get_length() const { return length_; } // Returns the length of the lwpolyline
		double get_area() const { return area_; } // Returns the area of the lwpolyline
		double is_drawn_ccw() const { return drawn_counter_clockwise_; }
		const lwpolyline* get_within_pointer() const { return within_pointer_; }
		// Set
		void set_elevation(const double new_elevation) // Sets the elevation of the lwpolyline
		{
			elevation_ = new_elevation;
		}

		void move_vertex(int id, const vertex& new_vertex); // Moves the vertex[id] to new location

		// Other functions
		bool within(const lwpolyline& pl);
		friend std::ostream& operator<<(std::ostream& os, lwpolyline);

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

		const lwpolyline* within_pointer_{ nullptr };

		/**
		 * \brief Function that calculates the total length & area of the polyline
		 */
		void calc_geometry();
	};
}
