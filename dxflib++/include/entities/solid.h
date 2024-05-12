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
		double x0{}, y0{}, z0{};
		double x1{}, y1{}, z1{};
		double x2{}, y2{}, z2{};

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
			x0 = 10,
			x1 = 11,
			x2 = 12,
			y0 = 20,
			y1 = 21,
			y2 = 22,
			z0 = 30,
			z1 = 31,
			z2 = 32,
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

	private:
		// Properties
		vertex v0_; // First corner
		vertex v1_; // Second corner
		vertex v2_; // Third corner
	};
}
