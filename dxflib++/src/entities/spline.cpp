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
		control_points_x.push_back(std::stod(nl));
		return 1;
	case spline_codes::y0:
		control_points_y.push_back(std::stod(nl));
		return 1;
	case spline_codes::z0:
		control_points_z.push_back(std::stod(nl));
		return 1;
	case spline_codes::x1:
		fit_points_x.push_back(std::stod(nl));
		return 1;
	case spline_codes::y1:
		fit_points_y.push_back(std::stod(nl));
		return 1;
	case spline_codes::z1:
		fit_points_z.push_back(std::stod(nl));
		return 1;
	case spline_codes::knot_values:
		knot_values.push_back(std::stod(nl));
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
	control_points_x.clear();
	control_points_y.clear();
	control_points_z.clear();
	knot_values.clear();
	fit_points_x.clear();
	fit_points_y.clear();
	fit_points_z.clear();
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
	for (int i = 0; i < number_of_control_points_; i++)
	{
		control_points_.push_back(vertex(sb.control_points_x[i], sb.control_points_y[i], sb.control_points_z[i]));
	}
	for (int i = 0; i < number_of_knots_; i++)
	{
		knot_values_.push_back(sb.knot_values[i]);
	}
	for (int i = 0; i < number_of_fit_points_; i++)
	{
		fit_points_.push_back(vertex(sb.fit_points_x[i], sb.fit_points_y[i], sb.fit_points_z[i]));
	}
}
