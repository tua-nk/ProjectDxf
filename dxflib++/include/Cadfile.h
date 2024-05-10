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
		const std::vector<entities::line>& get_lines() const { return lines_; }
		// lwpolylines
		const std::vector<entities::lwpolyline>& get_lwpolylines() const { return lwpolylines_; }
		// polylines
		const std::vector<entities::polyline>& get_polylines() const { return polylines_; }
		// hatches
		const std::vector<entities::hatch>& get_hatches() const { return hatches_; }
		// Text
		const std::vector<entities::text>& get_text() const { return basic_text_; }
		// Arc
		const std::vector<entities::arc>& get_arcs() const { return arcs_; }
		// Circle
		const std::vector<entities::circle>& get_circles() const { return circles_; }
		// Point
		const std::vector<entities::point_base>& get_points() const { return points_; }
		// Vertex
		const std::vector<entities::vertex>& get_vertexs() const { return vertexes_; }
		// Solid
		const std::vector<entities::solid>& get_solids() const { return solids_; }
		// Solid3d
		const std::vector<entities::solid3d>& get_solid3ds() const { return solid3ds_; }
		// Ellipse
		const std::vector<entities::ellipse>& get_ellipses() const { return ellipses_; }
		// Spline
		const std::vector<entities::spline>& get_splines() const { return splines_; }
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
		std::vector<entities::vertex> vertexes_; // VERTEX Entities
		std::vector<entities::solid> solids_; // SOLID Entities
		std::vector<entities::solid3d> solid3ds_; // 3DSOLID Entities
		std::vector<entities::ellipse> ellipses_; // ELLIPSE Entities
		std::vector<entities::spline> splines_; // SPLINE Entities

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
			const char* solid{ "SOLID" };
			const char* solid3d{ "3DSOLID" };
			const char* ellipse{ "ELLIPSE" };
			const char* spline{ "SPLINE" };
		};
	}
}
