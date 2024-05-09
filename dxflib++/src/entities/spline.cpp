#include "entities/spline.h"
#include "mathlib.h"

// splinebuf parse
/**
 * \brief Parse Function for the spline buffer
 * \param cl Current spline
 * \param nl Next spline
 * \return status -> 1: sucess, 0: no success
 */
int dxflib::entities::spline_buffer::parse(const std::string& cl, const std::string& nl)
{
	using namespace group_codes;
	int code{entity_buffer_base::parse(cl, nl)};
	if (code == -1)
		return 1;
	// Switch on the code and set the aproprate value in the spline buffer
	switch (static_cast<spline_codes>(code))
	{
	case spline_codes::error:
		return 0;
	case spline_codes::x0:
		x0 = std::stod(nl);
		return 1;
	case spline_codes::x1:
		x1 = std::stod(nl);
		return 1;
	case spline_codes::x2:
		x2 = std::stod(nl);
		return 1;
	case spline_codes::x3:
		x3 = std::stod(nl);
		return 1;
	case spline_codes::y0:
		y0 = std::stod(nl);
		return 1;
	case spline_codes::y1:
		y1 = std::stod(nl);
		return 1;
	case spline_codes::y2:
		y2 = std::stod(nl);
		return 1;
	case spline_codes::y3:
		y3 = std::stod(nl);
		return 1;
	case spline_codes::z0:
		z0 = std::stod(nl);
		return 1;
	case spline_codes::z1:
		z1 = std::stod(nl);
		return 1;
	case spline_codes::z2:
		z2 = std::stod(nl);
		return 1;
	case spline_codes::z3:
		z3 = std::stod(nl);
		return 1;
	case spline_codes::knot_value:
		knot_value = std::stod(nl);
		return 1;
	case spline_codes::weight_value:
		weight_value = std::stod(nl);
		return 1;
	case spline_codes::knot_tolerance:
		knot_tolerance = std::stod(nl);
		return 1;
	case spline_codes::control_point_tolerance:
		control_point_tolerance = std::stod(nl);
		return 1;
	case spline_codes::fit_tolerance:
		fit_tolerance = std::stod(nl);
		return 1;
	case spline_codes::flag:
		flag = std::stod(nl);
		return 1;
	case spline_codes::degree:
		degree = std::stod(nl);
		return 1;
	case spline_codes::number_of_knots:
		number_of_knots = std::stod(nl);
		return 1;
	case spline_codes::number_of_control_points:
		number_of_control_points = std::stod(nl);
		return 1;
	case spline_codes::number_of_fit_points:
		number_of_fit_points = std::stod(nl);
		return 1;
	default:
		return 0;
	}
}

void dxflib::entities::spline_buffer::free()
{
	entity_buffer_base::free();
	x0 = 0;
	y0 = 0;
	z0 = 0;
	x1 = 0;
	y1 = 0;
	z1 = 0;
	x2 = 0;
	y2 = 0;
	z2 = 0;
	x3 = 0;
	y3 = 0;
	z3 = 0;
	knot_value = 0;
	weight_value = 0;
	knot_tolerance = 0;
	control_point_tolerance = 0;
	fit_tolerance = 0;
	flag = 0;
	degree = 0;
	number_of_knots = 0;
	number_of_control_points = 0;
	number_of_fit_points = 0;
}

/**
 * \brief Spline buffer constructor for the spline entity
 * \param lb Spline Buffer
 */
dxflib::entities::spline::spline(spline_buffer& sb) :
	entity(sb),
	v0_(sb.x0, sb.y0, sb.z0),
	v1_(sb.x1, sb.y1, sb.z1),
	v2_(sb.x2, sb.y2, sb.z2),
	v3_(sb.x3, sb.y3, sb.z3),
	knot_value_(sb.knot_value),
	weight_value_(sb.weight_value),
	knot_tolerance_(sb.knot_tolerance),
	control_point_tolerance_(sb.control_point_tolerance),
	fit_tolerance_(sb.fit_tolerance),
	flag_(sb.flag),
	degree_(sb.degree),
	number_of_knots_(sb.number_of_knots),
	number_of_control_points_(sb.number_of_control_points),
	number_of_fit_points_(sb.number_of_fit_points)
{
}

const dxflib::entities::vertex& dxflib::entities::spline::get_vertex(const int id) const
{
	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	case 2: return v2_;
	case 3: return v3_;
	default: return v0_;
	}
}
