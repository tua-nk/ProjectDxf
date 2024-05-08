#include "entities/ellipse.h"
#include "mathlib.h"

// ellipsebuf parse
/**
 * \brief Parse Function for the ellipse buffer
 * \param cl Current ellipse
 * \param nl Next ellipse
 * \return status -> 1: sucess, 0: no success
 */
int dxflib::entities::ellipse_buffer::parse(const std::string& cl, const std::string& nl)
{
	using namespace group_codes;
	int code{entity_buffer_base::parse(cl, nl)};
	if (code == -1)
		return 1;
	// Switch on the code and set the aproprate value in the ellipse buffer
	switch (static_cast<ellipse_codes>(code))
	{
	case ellipse_codes::error:
		return 0;
	case ellipse_codes::thickness:
		thickness = std::stod(nl);
		return 1;
	case ellipse_codes::x0:
		x0 = std::stod(nl);
		return 1;
	case ellipse_codes::x1:
		x1 = std::stod(nl);
		return 1;
	case ellipse_codes::y0:
		y0 = std::stod(nl);
		return 1;
	case ellipse_codes::y1:
		y1 = std::stod(nl);
		return 1;
	case ellipse_codes::z0:
		z0 = std::stod(nl);
		return 1;
	case ellipse_codes::z1:
		z1 = std::stod(nl);
		return 1;
	default:
		return 0;
	}
}

void dxflib::entities::ellipse_buffer::free()
{
	entity_buffer_base::free();
	x0 = 0;
	y0 = 0;
	z0 = 0;
	x1 = 0;
	y1 = 0;
	z1 = 0;
	thickness = 0;
}

/**
 * \brief Ellipse buffer constructor for the ellipse entity
 * \param lb Ellipse Buffer
 */
dxflib::entities::ellipse::ellipse(ellipse_buffer& lb) :
	entity(lb),
	v0_(lb.x0, lb.y0, lb.z0),
	v1_(lb.x1, lb.y1, lb.z1),
	thickness_(lb.thickness),
	length_(mathlib::distance(v0_, v1_))
{
}

const dxflib::entities::vertex& dxflib::entities::ellipse::get_vertex(const int id) const
{
	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	default: return v0_;
	}
}

void dxflib::entities::ellipse::move_vertex(const int id, const vertex& new_vertex)
{
	switch (id)
	{
	case 0: v0_ = new_vertex;
	case 1: v1_ = new_vertex;
	default: ;
	}
	length_ = mathlib::distance(v0_, v1_);
}
