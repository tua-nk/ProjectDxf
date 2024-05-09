#pragma once
#include "entities/entity.h"
#include "entities/point.h"

namespace dxflib::entities
{
	/**
	* \brief Ellipse buffer struct
	*/
	struct ellipse_buffer : entity_buffer_base
	{
		double center_point_x{};
		double center_point_y{};
		double center_point_z{};
		double end_point_x{};
		double end_point_y{};
		double end_point_z{};
		double ratio{};
		double start_angle{};
		double end_angle{};

		/**
		 * \brief Parse function for ellipse buffer
		 * \param cl Current Ellipse
		 * \param nl Next Ellipse
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
		enum class ellipse_codes
		{
			subclassmarker = 100,
			error = -1,
			center_point_x = 10,
			center_point_y = 20,
			center_point_z = 30,
			end_point_x = 11,
			end_point_y = 21,
			end_point_z = 31,
			ratio = 40,
			start_angle = 41,
			end_angle = 42
		};
	}

	/**
	* \brief Ellipse Entity
	*/
	// ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor
	class ellipse : public entity
	{
	public:
		// Constructor
		explicit ellipse(ellipse_buffer&);

		// public interface
		const vertex& get_vertex(int id) const; // Returns the vertex given by it's id
		void move_vertex(int id, const vertex& new_vertex); // sets the vertex given by id to new location

		const vertex& get_center_point() const { return center_point_; } // Returns the Center Point
		const vertex& get_end_point() const { return center_point_; } // Returns the End Point
		double get_ratio() const { return ratio_; } // Ratio the Radius
		double get_start_angle() const { return start_angle_; } // Returns the Start parameter
		double get_end_angle() const { return end_angle_; } // Returns the End parameter

	private:
		// Properties
		vertex center_point_; // Starting Vertex
		vertex end_point_; // Ending Vertex
		double ratio_;
		double start_angle_;
		double end_angle_;
	};
}
