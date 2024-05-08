#pragma once
//#include "dxflib++/include/entities/line.h"
//#include "dxflib++/include/entities/lwpolyline.h"
#include "entities/line.h"
#include "entities/lwpolyline.h"
#include "entities/polyline.h"
#include "entities/arc.h"
#include "entities/circle.h"
#include "entities/hatch.h"
#include "entities/text.h"
#include "entities/solid3d.h"
#include "entities/solid.h"
#include "entities/ellipse.h"
#include "entities/spline.h"
#include <vector>

namespace dxflib
{
	/**
	 * \brief the main class for dxf files: contains all other objects
	 */
	class cadfile
	{
	public:
		/**
		 * \brief Constructor for the cadfile class
		 * \param path Path to the DXF file
		 */
		explicit cadfile(const char* path);

		// Public Interface
		// Lines
		std::vector<entities::line>& get_lines() { return lines_; }
		// lwpolylines
		std::vector<entities::lwpolyline>& get_lwpolylines() { return lwpolylines_; }
		// polylines
		std::vector<entities::polyline>& get_polylines() { return polylines_; }
		// hatches
		std::vector<entities::hatch>& get_hatches() { return hatches_; }
		// Text
		std::vector<entities::text>& get_text() { return basic_text_; }
		// Arc
		const std::vector<entities::arc>& get_arcs() const { return arcs_; }
		// Circle
		const std::vector<entities::circle>& get_circles() const { return circles_; }
		// Point
		const std::vector<entities::point_base>& get_points() const { return points_; }
		// Vertex
		const std::vector<entities::vertex>& get_vertexs() const { return vertex_; }
		// Filename of the DXF file
		std::string get_filename() const { return std::string{filename_}; }
		// DXF Data
		const std::vector<std::string>& get_data() const { return data_; }

	private:
		// Entities
		std::vector<entities::line> lines_; // LINE Entities
		std::vector<entities::lwpolyline> lwpolylines_; // LWPOLYLINE Entities
		std::vector<entities::polyline> polylines_; // LWPOLYLINE Entities
		std::vector<entities::hatch> hatches_; // HATCH Entities
		std::vector<entities::text> basic_text_; // TEXT Entities
		std::vector<entities::arc> arcs_; // ARC Entities
		std::vector<entities::circle> circles_; // CIRCLE Entities
		std::vector<entities::point_base> points_; // POINT Entities
		std::vector<entities::vertex> vertex_; // VERTEX Entities

		void read_file(); // Reads the file that is stored in filename_
		void parse_data(); // Main parse function for the dxf file: iterates through the data_ vector and
		void linker(); // Links entities to other entities, noteably hatches and polyline

		const char* filename_; // Path to the DXF file
		std::vector<std::string> data_; // raw data from the dxf file
	};

	// Group Codes
	namespace group_codes
	{
		/**
		 * \brief common group codes: for all entities
		 */
		struct g_common
		{
			const char* end_marker{"  0"};
		};

		/**
		 * \brief start markers for entities: AutoCAD LT 2013
		 */
		struct start_markers
		{
			const char* line{"LINE"};
			const char* lwpolyline{"LWPOLYLINE"};
			const char* polyline{ "POLYLINE" };
			const char* hatch{"HATCH"};
			const char* text{"TEXT"};
			const char* mtext{"MTEXT"};
			const char* arc{ "ARC" };
			const char* circle{ "CIRCLE" };
			const char* point{ "POINT" };
			const char* vertex{ "VERTEX" };
		};
	}
}
