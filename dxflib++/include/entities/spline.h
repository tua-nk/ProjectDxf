#pragma once
#include "entities/entity.h"
#include "entities/point.h"

namespace dxflib::entities
{
	/**
	* \brief Spline buffer struct
	*/
	struct spline_buffer : entity_buffer_base
	{
		double x0, y0, z0;
		double x1, y1, z1;
		double x2, y2, z2;
		double x3, y3, z3;
		double knot_value;
		double weight_value;
		double knot_tolerance;
		double control_point_tolerance;
		double fit_tolerance;
		int flag;
		int degree;
		int number_of_knots;
		int number_of_control_points;
		int number_of_fit_points;

		/**
		 * \brief Parse function for spline buffer
		 * \param cl Current Spline
		 * \param nl Next Spline
		 * \return Status 1:pass, 2: fail.
		 */
		int parse(const std::string& cl, const std::string& nl) override;
		/**
		 * \brief Frees memory and resets to defaults
		 */
		void free() override;
	};

	namespace group_codes
	{
		enum class spline_codes
		{
			subclassmarker = 100,
			error = -1,
			x0 = 10,
			x1 = 11,
			x2 = 12,
			x3 = 13,
			y0 = 20,
			y1 = 21,
			y2 = 22,
			y3 = 23,
			z0 = 30,
			z1 = 31,
			z2 = 32,
			z3 = 33,
			knot_value = 40,
			weight_value = 41,
			knot_tolerance = 42,
			control_point_tolerance = 43,
			fit_tolerance = 44,
			flag = 70,
			degree = 71,
			number_of_knots = 72,
			number_of_control_points = 73,
			number_of_fit_points = 74
		};
	}

	/**
	* \brief Spline Entity
	*/
	// ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor
	class spline : public entity
	{
	public:
		// Constructor
		explicit spline(spline_buffer&);

		// public interface
		const vertex& get_vertex(int id) const; // Returns the vertex given by it's id
		const double get_knot_value() const { return knot_value_;};
		const double get_weight_value() const { return weight_value_; };
		const double get_knot_tolerance() const { return knot_tolerance_; };
		const double get_control_point_tolerance() const { return control_point_tolerance_; };
		const double get_fit_tolerance() const { return fit_tolerance_; };
		const double get_flag() const { return flag_; };
		const double get_degree() const { return degree_; };
		const double get_number_of_knots() const { return number_of_knots_; };
		const double get_number_of_control_points() const { return number_of_control_points_; };
		const double get_number_of_fit_points() const { return number_of_fit_points_; };

	private:
		// Properties
		vertex v0_;
		vertex v1_;
		vertex v2_;
		vertex v3_;
		double knot_value_;
		double weight_value_;
		double knot_tolerance_;
		double control_point_tolerance_;
		double fit_tolerance_;
		int flag_;
		int degree_;
		int number_of_knots_;
		int number_of_control_points_;
		int number_of_fit_points_;
	};
}
