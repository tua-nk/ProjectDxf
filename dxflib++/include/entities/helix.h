#pragma once
#include "entities/entity.h"
#include "entities/point.h"

namespace dxflib::entities
{
	/**
	* \brief Solid buffer struct
	*/
	struct helix_bufer : entity_buffer_base
	{
		double x0{}, y0{}, z0{};
		double x1{}, y1{}, z1{};
		double x2{}, y2{}, z2{};
		double radius{};
		int number_of_turns{};
		double turn_height{};
		int constrain_type{};
		int handedness{};
		/**
		 * \brief Parse function for helix buffer
		 * \param cl Current Solid
		 * \param nl Next Solid
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
		enum class helix_codes
		{
			subclassmarker = 100,
			error = -1,
			x0 = 10,
			x1 = 11,
			x2 = 12,
			y0 = 20,
			y1 = 21,
			y2 = 22,
			z0 = 30,
			z1 = 31,
			z2 = 32,
			radius = 40,
			number_of_turns = 41,
			turn_height = 42,
			constrain_type = 280,
			handedness = 290
		};
	}

	/**
	* \brief Solid Entity
	*/
	// ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor
	class helix : public entity
	{
	public:
		// Constructor
		explicit helix(helix_bufer&);

		// public interface
		const vertex& get_axis_base_point() const { return axis_base_point_; };
		const vertex& get_start_point() const { return start_point_; };
		const vertex& get_axis_vector() const { return axis_vector_; };
		const double& get_radius() const { return radius_; };
		const int& get_number_of_turns() const { return number_of_turns_; };
		const double& get_turn_height() const { return turn_height_; };
		const int& get_constrain_type() const { return constrain_type_; };
		const int& get_handedness() const { return handedness_; };

	private:
		// Properties
		vertex axis_base_point_;
		vertex start_point_;
		vertex axis_vector_;
		double radius_;
		int number_of_turns_;
		double turn_height_;
		int constrain_type_;
		int handedness_;
	};
}
