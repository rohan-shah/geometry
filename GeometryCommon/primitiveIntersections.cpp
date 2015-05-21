#include "primitiveIntersections.hpp"
namespace geometry
{
	void intersect(const Circle& first, const Circle& second, std::vector<intersectionDescription>& result)
	{
		float sumRadius = (first.radius + second.radius);
		cml::vector2f centreDifference = first.pos - second.pos;
		float centreDifferenceSquared = centreDifference.length_squared();
		float centreDistance = centreDifference.length();
		if(sumRadius * sumRadius >= centreDifferenceSquared)
		{
			if(!(centreDistance + first.radius <= second.radius || centreDistance + second.radius <= first.radius))
			{
				//angle between the line joining the circles and the intersection point, at circle j
				float cosAngle = (first.radius * first.radius - second.radius * second.radius  - centreDifferenceSquared) / (-2 * second.radius * centreDistance);
				float angle = acos(cosAngle);
				float angleBetweenCentres = atan2(centreDifference[1], centreDifference[0]);
					
				float unitDirection1 = angleBetweenCentres + angle, unitDirection2 = angleBetweenCentres - angle;
					
				result.push_back(intersectionDescription(cml::vector2f(cos(unitDirection1), sin(unitDirection1))*second.radius + second.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&second))));
				result.push_back(intersectionDescription(cml::vector2f(cos(unitDirection2), sin(unitDirection2))*second.radius + second.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&second))));
			}
			//special case for one contained within the other. These are listed as self-intersections. 
			else if(centreDistance + first.radius < second.radius)
			{
				result.push_back(intersectionDescription(cml::vector2f(cos(0.0f), sin(0.0f))*first.radius + first.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&first))));
				result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI/2), sin((float)M_PI/2))*first.radius + first.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&first))));
				result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI), sin((float)M_PI))*first.radius + first.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&first))));
				result.push_back(intersectionDescription(cml::vector2f(cos(3*(float)M_PI/2), sin(3*(float)M_PI/2))*first.radius + first.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&first))));
			}
			else
			{
				result.push_back(intersectionDescription(cml::vector2f(cos(0.0f), sin(0.0f))*second.radius + second.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&second), static_cast<const Shape*>(&second))));
				result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI/2), sin((float)M_PI/2))*second.radius + second.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&second), static_cast<const Shape*>(&second))));
				result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI), sin((float)M_PI))*second.radius + second.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&second), static_cast<const Shape*>(&second))));
				result.push_back(intersectionDescription(cml::vector2f(cos(3*(float)M_PI/2), sin(3*(float)M_PI/2))*second.radius + second.pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&second), static_cast<const Shape*>(&second))));
			}
		}
	}
	void intersect(const Circle& first, const Line& second, std::vector<intersectionDescription>& result)
	{
		float closestPointParam = cml::dot(-second.pos + first.pos, second.unitDirection);
		cml::vector2f closestPoint = second.pos + closestPointParam*second.unitDirection;
		float closestDistanceSquared = (first.pos - closestPoint).length_squared();
		if(closestDistanceSquared < first.radius*first.radius)
		{
			float offset = sqrt(first.radius * first.radius - closestDistanceSquared);
			result.push_back(intersectionDescription(closestPoint + offset * second.unitDirection, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&second))));
			result.push_back(intersectionDescription(closestPoint - offset * second.unitDirection, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&first), static_cast<const Shape*>(&second))));
		}
	}
	void intersect(const Line& second, const Circle& first, std::vector<intersectionDescription>& result)
	{
		intersect(first, second, result);
	}
	void intersect(const Line& first, const Line& second, std::vector<intersectionDescription>& result)
	{
		bool found;
		cml::vector2f intersection = first.intersection(second, found);
		if(found)
		{
			result.push_back(intersectionDescription(intersection, intersectionDescription::intersectingObjects(&first, &second)));
		}
	}
}