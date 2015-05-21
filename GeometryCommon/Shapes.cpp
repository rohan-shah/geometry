#include "Shapes.h"
#include <algorithm>
namespace geometry
{
	Line::Line()
		:pos(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()), unitDirection(std::numeric_limits<float>::quiet_NaN(),std::numeric_limits<float>::quiet_NaN())
	{}
	cml::vector2f Line::intersection(const Line& other, bool& found) const
	{
		float inner = cml::dot(unitDirection, other.unitDirection);
		if(fabs(inner) > (1 - 0.0001)) 
		{
			found = false;
			return cml::vector2f(0, 0);
		}
		found = true;
		cml::vector2f thisPerp(this->unitDirection[1], -this->unitDirection[0]);
		float otherParam = cml::dot(-(this->pos - other.pos), thisPerp) / -cml::dot(other.unitDirection, thisPerp);
		cml::vector2f otherPos = other.pos + otherParam * other.unitDirection;
		return otherPos;
	}
	float Line::getRadius() const
	{
		return std::numeric_limits<float>::infinity();
	}
	float Circle::getRadius() const
	{
		return radius;
	}
	Line::Line(cml::vector2f pos, cml::vector2f unitDirection)
		:pos(pos), unitDirection(unitDirection)
	{
#ifndef NDEBUG
		if(fabs(unitDirection.length_squared() - 1) > 0.001)
		{
			throw std::runtime_error("Line direction must be normalised");
		}
#endif
	}
	bool intersectionDescription::operator==(const intersectionDescription& other) const
	{
		return (point == other.point) && ((objects.first == other.objects.first && objects.second == other.objects.second) || (objects.first == other.objects.second && objects.second == other.objects.first));
	}
	intersectionDescription::intersectionDescription(cml::vector2f point, intersectingObjects objects)
		:objects(objects), point(point)
	{}
	std::ostream& operator<<(std::ostream& os, const Line& line)
	{
		os << "geometry::Line(cml::vector2f(" << line.pos[0] << "f, " << line.pos[1] << "f), cml::vector2f(" << line.unitDirection[0] << "f, " << line.unitDirection[1] << "f))";
		return os;
	}
	std::ostream& operator<<(std::ostream& os, const Circle& circle)
	{
		os << "geometry::Circle(cml::vector2f(" << circle.pos[0] << "f, " << circle.pos[1] << "f), " << circle.radius << "f)";
		return os;
	}
}
