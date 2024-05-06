#include "entities/polyline.h"
#include "mathlib.h"
#include "entities/line.h"

int dxflib::entities::polyline_buffer::parse(const std::string& cl, const std::string& nl)
{
	int code{entity_buffer_base::parse(cl, nl)}; // group code of the current line if one exists
	if (code == -1)
		return 1;
	// parse swtich
	switch (static_cast<group_codes::polyline>(code))
	{
	case group_codes::polyline::x_value:
		bulge_values.push_back(geoline::bulge_null);
		x_values.push_back(std::stod(nl));
		return 1;

	case group_codes::polyline::y_value:
		y_values.push_back(std::stod(nl));
		return 1;

	case group_codes::polyline::polyline_flag:
		polyline_flag = std::stoi(nl);
		return 1;

	case group_codes::polyline::elevation:
		elevation = std::stod(nl);
		return 1;

	case group_codes::polyline::vertex_count:
		vertex_count = std::stoi(nl);
		return 1;

	case group_codes::polyline::bulge:
		/*
		 * A bulge null is pushed back at the extraction of an x value.
		 * When a bulge is found the bulge null is poped off and
		 * the actual bulge value is pushed back
		 */
		bulge_values.pop_back();
		bulge_values.push_back(std::stod(nl));
		return 1;

	case group_codes::polyline::starting_width:
		starting_width = std::stod(nl);
		return 1;

	case group_codes::polyline::ending_width:
		ending_width = std::stod(nl);
		return 1;

	case group_codes::polyline::width:
		width = std::stod(nl);
		return 1;

	case group_codes::polyline::vertex_id:
		// TODO: Add vertex_id
		return 1;

	default:
		return 0;
	}
}

void dxflib::entities::polyline_buffer::free()
{
	// Vector clearing
	x_values.clear();
	x_values.shrink_to_fit();
	y_values.clear();
	y_values.shrink_to_fit();
	bulge_values.clear();
	bulge_values.shrink_to_fit();

	// Property resetting
	polyline_flag = false;
	elevation = 0;
	vertex_count = 0;

	// Free the base class memory
	entity_buffer_base::free();
}

dxflib::entities::polyline::polyline(polyline_buffer& plb) :
	entity(plb),
	vertex_count_(plb.vertex_count),
	is_closed_(plb.polyline_flag),
	elevation_(plb.elevation),
	starting_width_(plb.starting_width),
	ending_width_(plb.ending_width), width_(plb.width),
	lines_(geoline::geoline_binder(
		plb.x_values, plb.y_values, plb.bulge_values,
		is_closed_))
{
	calc_geometry();
}

dxflib::entities::polyline::polyline(const line& l):
	entity(l),  // NOLINT(cppcoreguidelines-slicing)
	vertex_count_(2), is_closed_(false),
	elevation_((l.get_vertex(0).z + l.get_vertex(1).z) / 2),
	starting_width_(0), ending_width_(0), width_(l.get_thickness())
{
	lines_.emplace_back(l.get_vertex(0), l.get_vertex(1), geoline::bulge_null);
	calc_geometry();
}

void dxflib::entities::polyline::move_vertex(const int id, const vertex& new_vertex)
{
	lines_[id][0] = new_vertex;
	calc_geometry();
}

bool dxflib::entities::polyline::within(const polyline& pl)
{
	// A polyline cannot be within itself and the pl must be closed
	if (&pl == this || !pl.is_closed())
		return false;

	// If this object is known to be inside then return true
	if (&pl == within_pointer_)
		return true;

	int points_within{ 0 }; // Number of points within the boundary
	int total_points{ 0 };
	for (const auto& line : lines_)
	{
		if (line[0].within(pl))
			points_within++;
		total_points++;
	}

	// If the contender "pl" is within the current within object
	// then within precedence is used. Within_pointer is then changed
	if (within_pointer_ != nullptr)
	{
		int test_points_within{ 0 };
		int test_total_points{ 0 };
		for (const auto& line : pl.get_lines())
		{
			if (line[0].within(*within_pointer_))
				test_points_within++;
			test_total_points++;
		}
		if (test_points_within == test_total_points)
			within_pointer_ = &pl;
	}

	// Set the within pointe to the testing pl if non exists
	if (within_pointer_ == nullptr)
		within_pointer_ = &pl;

	return points_within == total_points;
}

void dxflib::entities::polyline::calc_geometry()
{
	double total_length{0};
	double total_area{0};

	// iterate through all geolines and return a total length & area
	for (auto& line : lines_)
	{
		total_length += line.get_length();
		total_area += line.get_area();
	}

	// set the total length of the polyline
	length_ = total_length;

	/*
	 * Note: Clockwise yeilds a positive area, Counterclockwise drawing yeilds a negative area.
	 * Since area is always positive though we take to abs() of the yeild
	 */
	if (area_ < 0)
		drawn_counter_clockwise_ = true;
	area_ = abs(total_area);
}
