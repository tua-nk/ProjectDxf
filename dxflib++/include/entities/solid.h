#pragma once
#include "entities/entity.h"
#include "entities/point.h"

namespace dxflib::entities
{
	/**
	* \brief Solid buffer struct
	*/
	struct solid_bufer : entity_buffer_base
	{
		double x0, y0, z0;
		double x1, y1, z1;
		double thickness;

		/**
		 * \brief Parse function for solid buffer
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
		enum class solid_codes
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
	* \brief Solid Entity
	*/
	// ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor
	class solid : public entity
	{
	public:
		// Constructor
		explicit solid(solid_bufer&);

		// public interface
		const vertex& get_vertex(int id) const; // Returns the vertex given by it's id
		void move_vertex(int id, const vertex& new_vertex); // sets the vertex given by id to new location
		double get_thickness() const { return thickness_; } // Returns the thickness of the solid
		double get_length() const { return length_; } // Returns the length of the solid
	private:
		// Properties
		vertex v0_; // Starting Vertex
		vertex v1_; // Ending Vertex
		double thickness_; // Thickness of the solid
		double length_; // distance from v0 to v1
	};
}
