#include "entities/lwpolyline.h"
#include "mathlib.h"
#include "entities/line.h"

dxflib::entities::geoline::geoline(const vertex& v0, const vertex& v1, const double bulge):
	v0_(v0), v1_(v1),
	bulge_(bulge)
{
	if (bulge_ == bulge_null) // If segment is a straight line
	{
		length_ = mathlib::distance(v0_, v1_);
		area_ = mathlib::trapz_area(v0_, v1_);
	}
	else // If segment is not a straight line
	{
		length_ = mathlib::distance(v0_, v1_, bulge_);
		total_angle_ = 4 * std::atan(bulge_);
		radius_ = mathlib::distance(v0_, v1_) / (2 * std::sin(total_angle_ / 2));
		area_ = mathlib::trapz_area(v0_, v1_) - mathlib::chord_area(radius_, total_angle_);
	}
}

std::vector<dxflib::entities::geoline> dxflib::entities::geoline::geoline_binder(const std::vector<double>& x,
                                                                                 const std::vector<double>& y,
                                                                                 const std::vector<double>& bulge,
                                                                                 const bool is_closed)
{
	// TODO: Add logging if exception is thrown
	if (x.size() != y.size() || x.size() != bulge.size())
		throw std::invalid_argument("Vectos must be the same size");

	// Geoline buffer
	std::vector<geoline> geolines;

	for (int pointnum{0}; pointnum < static_cast<int>(x.size()) - 1; ++pointnum)
	{
		const double b = bulge[pointnum];
		const double x0 = x[pointnum];
		const double x1 = x[pointnum + 1];
		const double y0 = y[pointnum];
		const double y1 = y[pointnum + 1];
		geolines.emplace_back(vertex{x0, y0}, vertex{x1, y1}, b);
	}
	// If the line is closed create one more line that extends from the last point to 
	// the starting point
	// BUG: Fails if x, y or bulge is empty
	if (is_closed && !x.empty() && !y.empty() && !bulge.empty())
		geolines.emplace_back(vertex{x.back(), y.back()}, vertex{x[0], y[0]}, bulge.back());
	return geolines;
}

double dxflib::entities::geoline::draw_direction(const geoline& p0_p1, const geoline& p1_p2)
{
	/*
	 * Function used to determine the draw direction of a set of geolines. This 
	 * is useful for determining if a bulge is convex or concave
	 */
	const mathlib::basic_vector v0{ p0_p1 };
	const mathlib::basic_vector v1{ p1_p2 };
	return mathlib::basic_vector::cross_product(v0, v1).z();
}

const dxflib::entities::vertex& dxflib::entities::geoline::operator[](const int id) const
{
	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	default: throw std::out_of_range("Only Two points in a geoline\n");
	}
}

/**
 * \brief Returns a vertex as a ref from a geoline
 * \param id Either 1 for the first vertex or two for the second
 * \return the vertex as a ref
 */
dxflib::entities::vertex& dxflib::entities::geoline::operator[](const int id)
{
	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	default: throw std::out_of_range("Only Two points in a geoline\n");
	}
}

inline double dxflib::entities::geoline::get_length() const
{
	if (bulge_ == bulge_null)
		return mathlib::distance(v0_, v1_);
	return mathlib::distance(v0_, v1_, bulge_);
}

inline double dxflib::entities::geoline::get_radius() const
{
	return bulge_ == static_cast<double>(bulge_null) ? std::numeric_limits<double>::infinity() : radius_;
}

double dxflib::entities::geoline::get_angle() const
{
	return bulge_ == static_cast<double>(bulge_null) ? std::numeric_limits<double>::infinity() : total_angle_;
}

int dxflib::entities::lwpolyline_buffer::parse(const std::string& cl, const std::string& nl)
{
	int code{entity_buffer_base::parse(cl, nl)}; // group code of the current line if one exists
	if (code == -1)
		return 1;
	// parse swtich
	switch (static_cast<group_codes::lwpolyline>(code))
	{
	case group_codes::lwpolyline::x_value:
		bulge_values.push_back(geoline::bulge_null);
		x_values.push_back(std::stod(nl));
		return 1;

	case group_codes::lwpolyline::y_value:
		y_values.push_back(std::stod(nl));
		return 1;

	case group_codes::lwpolyline::polyline_flag:
		polyline_flag = std::stoi(nl);
		return 1;

	case group_codes::lwpolyline::elevation:
		elevation = std::stod(nl);
		return 1;

	case group_codes::lwpolyline::vertex_count:
		vertex_count = std::stoi(nl);
		return 1;

	case group_codes::lwpolyline::bulge:
		/*
		 * A bulge null is pushed back at the extraction of an x value.
		 * When a bulge is found the bulge null is poped off and
		 * the actual bulge value is pushed back
		 */
		bulge_values.pop_back();
		bulge_values.push_back(std::stod(nl));
		return 1;

	case group_codes::lwpolyline::starting_width:
		starting_width = std::stod(nl);
		return 1;

	case group_codes::lwpolyline::ending_width:
		ending_width = std::stod(nl);
		return 1;

	case group_codes::lwpolyline::width:
		width = std::stod(nl);
		return 1;

	case group_codes::lwpolyline::vertex_id:
		// TODO: Add vertex_id
		return 1;

	default:
		return 0;
	}
}

void dxflib::entities::lwpolyline_buffer::free()
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

dxflib::entities::lwpolyline::lwpolyline(lwpolyline_buffer& lwb) :
	entity(lwb),
	vertex_count_(lwb.vertex_count),
	is_closed_(lwb.polyline_flag),
	elevation_(lwb.elevation),
	starting_width_(lwb.starting_width),
	ending_width_(lwb.ending_width), width_(lwb.width),
	lines_(geoline::geoline_binder(
		lwb.x_values, lwb.y_values, lwb.bulge_values,
		is_closed_))
{
	calc_geometry();
}

dxflib::entities::lwpolyline::lwpolyline(const line& l):
	entity(l),  // NOLINT(cppcoreguidelines-slicing)
	vertex_count_(2), is_closed_(false),
	elevation_((l.get_vertex(0).z + l.get_vertex(1).z) / 2),
	starting_width_(0), ending_width_(0), width_(l.get_thickness())
{
	lines_.emplace_back(l.get_vertex(0), l.get_vertex(1), geoline::bulge_null);
	calc_geometry();
}

void dxflib::entities::lwpolyline::move_vertex(const int id, const vertex& new_vertex)
{
	lines_[id][0] = new_vertex;
	calc_geometry();
}

bool dxflib::entities::lwpolyline::within(const lwpolyline& pl)
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

void dxflib::entities::lwpolyline::calc_geometry()
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

std::ostream& dxflib::entities::operator<<(std::ostream& os, geoline& geoline)
{
	os << "Start vertex: " << geoline.v0_ << ", End Vertex: " << geoline.v1_;
	return os;
}

std::ostream& dxflib::entities::operator<<(std::ostream& os, lwpolyline lw)
{
	os << "Layer: " << lw.layer_ << "\n";
	for (size_t i{0}; i < lw.lines_.size(); ++i)
	{
		os << "Line Number: " << i << " " << lw.lines_[i] << "\n";
	}
	os << "Length: " << lw.length_ << "\n";
	return os;
}
