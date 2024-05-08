#pragma once
#include "entities/entity.h"
#include "entities/point.h"

namespace dxflib::entities
{
	/**
	* \brief 3dsolid buffer struct
	*/
	struct 3dsolid_buf : entity_buffer_base
	{
		double x0, y0, z0;
		double x1, y1, z1;
		double thickness;

		/**
		 * \brief Parse function for 3dsolid buffer
		 * \param cl Current 3dsolid
		 * \param nl Next 3dsolid
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
		enum class 3dsolid_codes
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
	* \brief 3dsolid Entity
	*/
	// ReSharper disable CppPolymorphicClassWithNonVirtualPublicDestructor
	class 3dsolid : public entity
	{
	public:
		// Constructor
		explicit 3dsolid(3dsolid_buf&);

		// public interface
		const vertex& get_vertex(int id) const; // Returns the vertex given by it's id
		void move_vertex(int id, const vertex& new_vertex); // sets the vertex given by id to new location
		double get_thickness() const { return thickness_; } // Returns the thickness of the 3dsolid
		lwpoly3dsolid to_lwpoly3dsolid() const; // Converts 3dsolid to a lwpoly3dsolid
		double get_length() const { return length_; } // Returns the length of the 3dsolid
	private:
		// Properties
		vertex v0_; // Starting Vertex
		vertex v1_; // Ending Vertex
		double thickness_; // Thickness of the 3dsolid
		double length_; // distance from v0 to v1
	};
}
