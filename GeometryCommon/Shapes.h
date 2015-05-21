#ifndef SHAPES_HEADER_GUARD
#define SHAPES_HEADER_GUARD
#include <vector>
#include <utility>
#include <cml/cml.h>
namespace geometry
{
	struct Shape;
	struct intersectionDescription
	{
		typedef std::pair<const Shape*, const Shape*> intersectingObjects;
		intersectingObjects objects;
		cml::vector2f point;
		intersectionDescription(cml::vector2f point, intersectingObjects objects);
		bool operator==(const intersectionDescription& other) const;
		struct lexographicSorter
		{
			bool operator()(const intersectionDescription& first, const intersectionDescription& second) const
			{
				return first.point < second.point;
			}
		};
		struct angleSorter
		{
			cml::vector2f initialPoint;
			angleSorter(cml::vector2f initialPoint)
				:initialPoint(initialPoint)
			{}
			bool operator()(const intersectionDescription& first, const intersectionDescription& second) const
			{
				cml::vector2f firstDiff = first.point - initialPoint;
				cml::vector2f secondDiff = second.point - initialPoint;
				return atan2(firstDiff[1], firstDiff[0]) < atan2(secondDiff[1], secondDiff[0]);
			}
		};
	};
	struct Shape
	{
		virtual float getRadius() const =0;
	};
	struct Circle : public Shape
	{
		typedef cml::vector2f vector_type;
		typedef vector_type::value_type element_type;
		Circle(element_type x, element_type y, float radius)
			:pos(x, y), radius(radius)
		{};
		Circle(Circle&& other)
			:pos(other.pos), radius(other.radius)
		{}
		Circle& operator=(const Circle& other)
		{
			pos = other.pos;
			radius = other.radius;
			return *this;
		}
		Circle(const Circle& other)
			:pos(other.pos), radius(other.radius)
		{}
		Circle(vector_type centre, float radius)
			:pos(centre), radius(radius)
		{};
		virtual float getRadius() const;
		vector_type pos;
		float radius;
	};
	struct Line : public Shape
	{
		Line(cml::vector2f pos, cml::vector2f unitDirection);
		Line();
		cml::vector2f pos, unitDirection;
		cml::vector2f intersection(const Line& other, bool& found) const;
		virtual float getRadius() const;
		bool correctSide(cml::vector2f testPoint) const
		{
			cml::vector2f normal(unitDirection[1], -unitDirection[0]);
			return cml::dot(normal, testPoint - pos) < 0;
		}
	};
	struct SemiCircle
	{
		typedef cml::vector2f vector_type;
		typedef vector_type::value_type element_type;
		SemiCircle(vector_type centre, vector_type straight_line_direction, float radius)
			:circle(centre, radius), straightLine(centre, straight_line_direction)
		{}
		Circle circle;
		Line straightLine;
	};
	std::ostream& operator<<(std::ostream& os, const Line& line);
	std::ostream& operator<<(std::ostream& os, const Circle& circle);
}
#endif
