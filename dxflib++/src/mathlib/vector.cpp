#include "mathlib.h"
#include "entities/lwpolyline.h"
#include "entities/line.h"

dxflib::mathlib::basic_vector::basic_vector(const entities::vertex& v0, const entities::vertex& v1):
	v0_(v0), v1_(v1), x_(v1_.x - v0_.x), y_(v1_.y - v0_.y), z_(v1.z - v0.z), has_verticies_(true)
{
}

dxflib::mathlib::basic_vector::basic_vector(const double x, const double y, const double z):
	v0_(0), v1_(0), x_(x), y_(y), z_(z), has_verticies_(false)
{
}

dxflib::mathlib::basic_vector::basic_vector(const entities::geoline& gl) :
	v0_(gl[0]), v1_(gl[1]), x_(v1_.x - v0_.x), y_(v1_.y - v0_.y),
	z_(v1_.z - v0_.z), has_verticies_(true)
{
}

dxflib::entities::vertex& dxflib::mathlib::basic_vector::operator[](const int id)
{
	if (!has_verticies_)
		throw exceptions::no_associated_vertex("No Associated Vertex with Vector");

	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	default: throw std::invalid_argument("Invalid vertex ID");
	}
}

double dxflib::mathlib::basic_vector::magnitude() const
{
	if (has_verticies_)
		return distance(v0_, v1_);
	return sqrt(pow(x_, 2.0) + pow(y_, 2.0) + pow(z_, 2.0));
}

const dxflib::entities::vertex& dxflib::mathlib::basic_vector::operator[](const int id) const
{
	if (!has_verticies_)
		throw exceptions::no_associated_vertex("No Associated Vertex with Vector");

	switch (id)
	{
	case 0: return v0_;
	case 1: return v1_;
	default: throw std::invalid_argument("Invalid vertex ID");
	}
}

inline dxflib::mathlib::basic_vector dxflib::mathlib::basic_vector::operator+(const basic_vector& v) const
{
	return basic_vector{
		entities::vertex{v.v0_.x + v0_.x, v.v0_.y + v0_.y, v.v0_.z + v0_.z},
		entities::vertex{v.v1_.x + v1_.x, v.v1_.y + v1_.y, v.v1_.z + v1_.z}
	};
}

inline dxflib::mathlib::basic_vector dxflib::mathlib::basic_vector::operator-(const basic_vector& v) const
{
	return basic_vector{
		entities::vertex{v.v0_.x - v0_.x, v.v0_.y - v0_.y, v.v0_.z - v0_.z},
		entities::vertex{v.v1_.x - v1_.x, v.v1_.y - v1_.y, v.v1_.z - v1_.z}
	};
}

double dxflib::mathlib::basic_vector::dot_product(const basic_vector& v0, const basic_vector& v1)
{
	return v1.x() * v0.x() + v1.y() * v0.y() + v1.z() * v0.z();
}

dxflib::mathlib::basic_vector dxflib::mathlib::basic_vector::cross_product(const basic_vector& v0,
                                                                           const basic_vector& v1)
{
	return basic_vector{
		v0.y() * v1.z() - v1.y() * v0.z(),
		-(v0.x() * v1.z() - v1.x() * v0.z()),
		v0.x() * v1.y() - v1.x() * v0.y()
	};
}

int dxflib::mathlib::winding_num(const std::vector<entities::geoline>& geolines, const entities::vertex& v)
{
	/*
	 * This function uses the winding method to return the winding number of a polygon. 
	 * The winding number of a polygon is used to determing if a point is within a polygon.
	 * If the winding number is != 0 then the point is within the polygon. 
	 * 
	 * Method:
	 *		First the boundary conditions are checked. 1) the geoline segment is above the point
	 *												   2) the geoline segment is below the point
	 *												   3) the geoline segment is to the right of the point
	 *		Then a cross product between the x unit vector and the line vector is taken. The resultant z component
	 *		is then used to determine if the geoline segment is an upcrossing (cp > 1) or a downcrossing (cp <= 1).
	 *		The sum is either added to or subtracted by one respectivly. 
	 *		
	 * Return:
	 *		The return of this function is the winding number. The point is inside the polyline if the winding number
	 *		is non-zero.
	 */

	int sum{0};
	bool is_ccw{ entities::geoline::draw_direction(geolines[0], geolines[1]) > 0 };
	for (const auto& line : geolines)
	{
		// Loop constants
		const basic_vector l0_l1{line}; // vector from v0 to v1 of the geoline
		bool in_arc{ false };

		// if the line is an arc then test to see if the point is within the arc segments zone of influence
		if (line.get_bulge() != entities::geoline::bulge_null)
			in_arc = is_within_arc(line[0], v, line[1], line.get_angle());
		if (line[0].y > v.y && line[1].y > v.y && !in_arc) // Line is above
			continue;
		if (line[0].y < v.y && line[1].y < v.y && !in_arc) // Line is below
			continue;
		if (line[0].x < v.x && line[1].x < v.x && !in_arc) // Line is to the right
			continue;

		bool is_convex{ false };
		if ((is_ccw && line.get_bulge() > 0) || (!is_ccw && line.get_bulge() < 0))
			is_convex = true;

		// Vectors
		const basic_vector l0_v{ line[0], v };
		const double l0_l1_x_l0_v{ basic_vector::cross_product(l0_l1, l0_v).z() };

		const basic_vector x_unit_vector{1, 0, 0}; // Unit vector in the x direction
		const double x_unit_vector_cross_l0_l1{basic_vector::cross_product(x_unit_vector, l0_l1).z()};

		/*
		 * There is a special case where the point is to the left of the line but is not left of both
		 * points of the line. To determine if the point is in this condition we need to compare the two cross
		 * products that we calculated above. The signs will differ if 
		 */
		// Line is to the right and the point is not within an arc and meets the special condition above
		if (l0_l1_x_l0_v*x_unit_vector_cross_l0_l1 < 0 && !in_arc)
			continue;
		// Line is to the right and point is within a concave arc
		if (l0_l1_x_l0_v*x_unit_vector_cross_l0_l1 > 0 && in_arc && !is_convex)
			continue;
		// Line is to the left and the point is within a convex arc
		if (l0_l1_x_l0_v*x_unit_vector_cross_l0_l1 > 0 && in_arc && is_convex)
			continue;
		
		if (x_unit_vector_cross_l0_l1 < 0) // downcrossing
			sum -= 1;
		else        // upcrossing
			sum += 1;
	}
	return sum;
}

bool dxflib::mathlib::is_within_arc(const entities::vertex& p1, const entities::vertex& p, const entities::vertex& p2,
                                    const double total_angle)
{
	// TODO: add comment about how this works
	/*
	 * Is within arc function:
	 *  
	 */

	const basic_vector p1_p{p1, p};
	const basic_vector p1_p2{p1, p2};
	const basic_vector p2_p{p2, p};
	const basic_vector p2_p1{p2, p1};

	const double phi_1{acos(basic_vector::dot_product(p1_p, p1_p2) / (p1_p.magnitude() * p1_p2.magnitude()))};
	const double phi_2{acos(basic_vector::dot_product(p2_p, p2_p1) / (p2_p.magnitude() * p2_p1.magnitude()))};

	const double sum{phi_1 + phi_2};
	const double bulge{ tan(total_angle / 4) };
	if (basic_vector::cross_product(p1_p, p1_p2).z()*bulge < 0)
		return false;

	return sum < abs(total_angle) / 2;
}

std::ostream& dxflib::mathlib::operator<<(std::ostream& os, const basic_vector& bv)
{
	os << "[" << bv.x() << "i, " << bv.y() << "j, " << bv.z() << "k]";
	return os;
}
