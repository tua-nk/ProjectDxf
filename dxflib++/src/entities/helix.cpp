#include "entities/helix.h"
#include "mathlib.h"

// helixbuf parse
/**
 * \brief Parse Function for the helix buffer
 * \param cl Current helix
 * \param nl Next helix
 * \return status -> 1: sucess, 0: no success
 */
int dxflib::entities::helix_bufer::parse(const std::string& cl, const std::string& nl)
{
	using namespace group_codes;
	int code{entity_buffer_base::parse(cl, nl)};
	if (code == -1)
		return 1;
	// Switch on the code and set the aproprate value in the helix buffer
	switch (static_cast<helix_codes>(code))
	{
	case helix_codes::error:
		return 0;
	case helix_codes::x0:
		x0 = std::stod(nl);
		return 1;
	case helix_codes::x1:
		x1 = std::stod(nl);
		return 1;
	case helix_codes::x2:
		x2 = std::stod(nl);
		return 1;
	case helix_codes::y0:
		y0 = std::stod(nl);
		return 1;
	case helix_codes::y1:
		y1 = std::stod(nl);
		return 1;
	case helix_codes::y2:
		y2 = std::stod(nl);
		return 1;
	case helix_codes::z0:
		z0 = std::stod(nl);
		return 1;
	case helix_codes::z1:
		z1 = std::stod(nl);
		return 1;
	case helix_codes::z2:
		z2 = std::stod(nl);
		return 1;
	case helix_codes::radius:
		radius = std::stod(nl);
		return 1;
	case helix_codes::number_of_turns:
		number_of_turns = std::stod(nl);
		return 1;
	case helix_codes::turn_height:
		turn_height = std::stod(nl);
		return 1;
	case helix_codes::constrain_type:
		constrain_type = std::stod(nl);
		return 1;
	case helix_codes::handedness:
		handedness = std::stod(nl);
		return 1;
	default:
		return 0;
	}
}

void dxflib::entities::helix_bufer::free()
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
	radius = 0;
	number_of_turns = 0;
	turn_height = 0;
	constrain_type = 0;
	handedness = 0;
}

/**
 * \brief Solid buffer constructor for the helix entity
 * \param lb Solid Buffer
 */
dxflib::entities::helix::helix(helix_bufer& sb) :
	entity(sb),
	axis_base_point_(sb.x0, sb.y0, sb.z0),
	start_point_(sb.x1, sb.y1, sb.z1),
	axis_vector_(sb.x2, sb.y2, sb.z2),
	radius_(sb.radius),
	number_of_turns_(sb.number_of_turns),
	turn_height_(sb.turn_height),
	constrain_type_(sb.constrain_type),
	handedness_(sb.handedness)
	
{
}
