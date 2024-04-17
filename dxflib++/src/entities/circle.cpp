/*
 * circle.cpp
 * By: Adam Renaud
 * 2018
 * Purpose: To define the decarlations made in the circle.h file for the circle_buffer and the
 * circle enity class
 */
#include "entities/circle.h"
#include "mathlib.h"
#include <valarray>

int dxflib::entities::circle_buffer::parse(const std::string& cl, const std::string& nl)
{
	int code{entity_buffer_base::parse(cl, nl)}; // group code of the current line
	if (code == -1)
		return 1;
	// Parse switch;
	switch (static_cast<group_codes::circle>(code))
	{
	case group_codes::circle::thickness:
		thickness = std::stod(nl);
		return 1;

	case group_codes::circle::center_point_x:
		center_point_x = std::stod(nl);
		return 1;

	case group_codes::circle::center_point_y:
		center_point_y = std::stod(nl);
		return 1;

	case group_codes::circle::center_point_z:
		center_point_z = std::stod(nl);
		return 1;

	case group_codes::circle::radius:
		radius = std::stod(nl);
		return 1;

	default:
		return 0;
	}
}

void dxflib::entities::circle_buffer::free()
{
	entity_buffer_base::free();
	thickness = 0;
	center_point_x = 0;
	center_point_y = 0;
	center_point_z = 0;
	radius = 0;
}

dxflib::entities::circle::circle(circle_buffer& ab): entity(ab),
	center_point_(ab.center_point_x, ab.center_point_y, ab.center_point_z),
	radius_(ab.radius), thickness_(ab.thickness)
{

}
