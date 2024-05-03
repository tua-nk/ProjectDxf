#pragma once
#include <ostream>
#include <vector>
#include "entities/entity.h"
namespace dxflib::entities
{
	class lwpolyline;
	class geoline;

	namespace group_codes
	{
		enum class point
		{
			point_x = 10,
			point_y = 20,
			point_z = 30,
		};
	}

	struct point_buffer : virtual entity_buffer_base
	{
		double point_x{};
		double point_y{};
		double point_z{};

		int parse(const std::string& cl, const std::string& nl) override;
		void free() override;
	};

	/**
	* \brief point base class
	*/
	class point_base
	{
	public:
		// Constructor
		explicit point_base(point_buffer& pb); // buffer Constructor

		double x, y, z;
		explicit point_base(double x = 0, double y = 0, double z = 0);
		friend std::ostream& operator<<(std::ostream& os, point_base pb);
	};

	/**
	* \brief Vertex Class
	*/
	class vertex : public point_base
	{
	public:
		// Constructor
		explicit vertex(point_buffer& pb); // buffer Constructor
		/**
		* \brief Vertex Object is a sinlge point defined by: x, y, z
		* \param x x position
		* \param y y position
		* \param z z position
		*/
		explicit vertex(double x = 0, double y = 0, double z = 0);
		bool within(const lwpolyline& pl) const;
		bool within(const std::vector<geoline>& gl_vector) const;
		bool operator==(const vertex& other) const;
	};
}
