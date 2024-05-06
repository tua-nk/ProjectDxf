#include "entities/point.h"
#include "entities/lwpolyline.h"
#include "entities/polyline.h"
#include "mathlib.h"
#include <ostream>

dxflib::entities::point_base::point_base(point_buffer& pb):
	x(pb.point_x), y(pb.point_y), z(pb.point_z)
{
}

dxflib::entities::point_base::point_base(const double x, const double y, const double z):
	x(x), y(y), z(z)
{
}

dxflib::entities::vertex::vertex(const double x, const double y, const double z) :
	point_base(x, y, z)
{
}

int dxflib::entities::point_buffer::parse(const std::string& cl, const std::string& nl)
{
	int code{entity_buffer_base::parse(cl, nl)}; // group code of the current line
	if (code == -1)
		return 1;
	// Parse switch;
	switch (static_cast<group_codes::point>(code))
	{
	case group_codes::point::point_x:
		point_x = std::stod(nl);
		return 1;

	case group_codes::point::point_y:
		point_y = std::stod(nl);
		return 1;

	case group_codes::point::point_z:
		point_z = std::stod(nl);
		return 1;

	default:
		return 0;
	}
}

void dxflib::entities::point_buffer::free()
{
	entity_buffer_base::free();
	point_x = 0;
	point_y = 0;
	point_z = 0;
}

dxflib::entities::vertex::vertex(point_buffer& pb) :point_base(pb)
{
}

bool dxflib::entities::vertex::within(const lwpolyline& pl) const
{
	if (!pl.is_closed())
		return false;
	return mathlib::winding_num(pl.get_lines(), *this) != 0;
}

bool dxflib::entities::vertex::within(const polyline& pl) const
{
	if (!pl.is_closed())
		return false;
	return mathlib::winding_num(pl.get_lines(), *this) != 0;
}

bool dxflib::entities::vertex::within(const std::vector<geoline>& gl_vector) const
{
	return mathlib::winding_num(gl_vector, *this) != 0;
}

bool dxflib::entities::vertex::operator==(const vertex& other) const
{
	return other.x == x && other.y == y && other.z == z;
}

std::ostream& dxflib::entities::operator<<(std::ostream& os, const point_base pb)
{
	os << "[" << pb.x << ", " << pb.y << ", " << pb.z << "]";
	return os;
}
