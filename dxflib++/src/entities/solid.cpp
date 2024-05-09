#include "entities/solid.h"
#include "mathlib.h"

// solidbuf parse
/**
 * \brief Parse Function for the solid buffer
 * \param cl Current solid
 * \param nl Next solid
 * \return status -> 1: sucess, 0: no success
 */
int dxflib::entities::solid_bufer::parse(const std::string& cl, const std::string& nl)
{
	using namespace group_codes;
	int code{entity_buffer_base::parse(cl, nl)};
	if (code == -1)
		return 1;
	// Switch on the code and set the aproprate value in the solid buffer
	switch (static_cast<solid_codes>(code))
	{
	case solid_codes::error:
		return 0;
	case solid_codes::x0:
		x0 = std::stod(nl);
		return 1;
	case solid_codes::x1:
		x1 = std::stod(nl);
		return 1;
	case solid_codes::x2:
		x2 = std::stod(nl);
		return 1;
	case solid_codes::y0:
		y0 = std::stod(nl);
		return 1;
	case solid_codes::y1:
		y1 = std::stod(nl);
		return 1;
	case solid_codes::y2:
		y2 = std::stod(nl);
		return 1;
	case solid_codes::z0:
		z0 = std::stod(nl);
		return 1;
	case solid_codes::z1:
		z1 = std::stod(nl);
		return 1;
	case solid_codes::z2:
		z2 = std::stod(nl);
		return 1;
	default:
		return 0;
	}
}

void dxflib::entities::solid_bufer::free()
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
}

/**
 * \brief Solid buffer constructor for the solid entity
 * \param lb Solid Buffer
 */
dxflib::entities::solid::solid(solid_bufer& sb) :
	entity(sb),
	v0_(sb.x0, sb.y0, sb.z0),
	v1_(sb.x1, sb.y1, sb.z1),
	v2_(sb.x2, sb.y2, sb.z2)
{
}

const dxflib::entities::vertex& dxflib::entities::solid::get_vertex(const int id) const
{
	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	case 2: return v2_;
	default: return v0_;
	}
}

