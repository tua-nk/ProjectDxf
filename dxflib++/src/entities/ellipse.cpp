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
	case ellipse_codes::center_point_x:
		center_point_x = std::stod(nl);
		return 1;
	case ellipse_codes::center_point_y:
		center_point_y = std::stod(nl);
		return 1;
	case ellipse_codes::center_point_z:
		center_point_z = std::stod(nl);
		return 1;
	case ellipse_codes::end_point_x:
		end_point_x = std::stod(nl);
		return 1;
	case ellipse_codes::end_point_y:
		end_point_y = std::stod(nl);
		return 1;
	case ellipse_codes::end_point_z:
		end_point_z = std::stod(nl);
		return 1;
	case ellipse_codes::ratio:
		ratio = std::stod(nl);
		return 1;
	case ellipse_codes::start_angle:
		start_angle = std::stod(nl);
		return 1;
	case ellipse_codes::end_angle:
		end_angle = std::stod(nl);
		return 1;
	default:
		return 0;
	}
}

void dxflib::entities::ellipse_buffer::free()
{
	entity_buffer_base::free();
	center_point_x = 0;
	center_point_y = 0;
	center_point_z = 0;
	end_point_x = 0;
	end_point_y = 0;
	end_point_z = 0;
	ratio = 0;
	start_angle = 0;
	end_angle = 0;
}

/**
 * \brief Ellipse buffer constructor for the ellipse entity
 * \param lb Ellipse Buffer
 */
dxflib::entities::ellipse::ellipse(ellipse_buffer& eb) :
	entity(eb),
	center_point_(eb.center_point_x, eb.center_point_y, eb.center_point_z),
	end_point_(eb.end_point_x, eb.end_point_y, eb.end_point_z),
	ratio_(eb.ratio),
	start_angle_(eb.start_angle),
	end_angle_(eb.end_angle)
{
}