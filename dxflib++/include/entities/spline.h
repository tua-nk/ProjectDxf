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
		std::vector<double> control_points_x;    //10
		std::vector<double> control_points_y;    //20
		std::vector<double> control_points_z;    //30
		std::vector<double> knot_values;         //40
		std::vector<double> fit_points_x;        //11
		std::vector<double> fit_points_y;        //21
		std::vector<double> fit_points_z;        //31
		double weight_value;                     //41
		double knot_tolerance;                   //42
		double control_point_tolerance;          //43
		double fit_tolerance;                    //44
		int flag;                                //70
		int degree;                              //71
		int number_of_knots;                     //72
		int number_of_control_points;            //73
		int number_of_fit_points;                //74

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
			y0 = 20,
			z0 = 30,
			x1 = 11,
			y1 = 21,
			z1 = 31,
			knot_values = 40,
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
		const std::vector<vertex> get_control_points() const { return control_points_; };
		const std::vector<double> get_knot_value() const { return knot_values_; };
		const std::vector<vertex> get_fit_points() const { return fit_points_; };
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
		std::vector<vertex> control_points_;
		std::vector<double> knot_values_;
		std::vector<vertex> fit_points_;
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
