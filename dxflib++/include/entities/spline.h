#pragma once
#include "entities/entity.h"
#include "entities/point.h"

namespace dxflib::entities
{
	/**
	* \brief Spline buffer struct
	*/
	struct spline_buf : entity_buffer_base
	{
		double x0, y0, z0;
		double x1, y1, z1;
		double thickness;

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
			thickness = 39,
			x0 = 10,
			x1 = 11,
			y0 = 20,
			y1 = 21,
			z0 = 30,
			z1 = 31,
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
		explicit spline(spline_buf&);

		// public interface
		const vertex& get_vertex(int id) const; // Returns the vertex given by it's id
		void move_vertex(int id, const vertex& new_vertex); // sets the vertex given by id to new location
		double get_thickness() const { return thickness_; } // Returns the thickness of the spline
		double get_length() const { return length_; } // Returns the length of the spline
	private:
		// Properties
		vertex v0_; // Starting Vertex
		vertex v1_; // Ending Vertex
		double thickness_; // Thickness of the spline
		double length_; // distance from v0 to v1
	};
}
