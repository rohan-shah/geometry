#ifndef PRIMITIVE_INTERSECTIONS_HEADER_GUARD
#define PRIMITIVE_INTERSECTIONS_HEADER_GUARD
#include "Shapes.h"
namespace geometry
{
	template<class T> void intersect(const T& other, const SemiCircle& semi, std::vector<intersectionDescription>& result)
	{
		std::vector<intersectionDescription> circleIntersections;
		//circle / line intersections
		intersect(semi.circle, other, circleIntersections);
		//remove ones that are in the wrong half of the semicircle
		std::vector<intersectionDescription>::iterator tmpIterator = circleIntersections.begin();
		while(tmpIterator != circleIntersections.end())
		{
			if(!semi.straightLine.correctSide(tmpIterator->point))
			{
				tmpIterator = circleIntersections.erase(tmpIterator);
			}
			else
			{
				tmpIterator++;
			}
		}
		
		//now the intersections with the line, which must also be within radius of the centre. 
		std::vector<intersectionDescription> lineIntersections;
		intersect(semi.straightLine, other, lineIntersections);
		tmpIterator = lineIntersections.begin();
		while(tmpIterator != lineIntersections.end())
		{
			if((tmpIterator->point - semi.circle.pos).length_squared() > semi.circle.radius * semi.circle.radius)
			{
				tmpIterator = lineIntersections.erase(tmpIterator);
			}
			else
			{
				tmpIterator++;
			}
		}
		result.insert(result.end(), circleIntersections.begin(), circleIntersections.end());
		result.insert(result.end(), lineIntersections.begin(), lineIntersections.end());
	}
	void intersect(const Circle& first, const Circle& second, std::vector<intersectionDescription>& result);
	void intersect(const Circle& first, const Line& second, std::vector<intersectionDescription>& result);
	void intersect(const Line& second, const Circle& first, std::vector<intersectionDescription>& result);
	void intersect(const Line& first, const Line& second, std::vector<intersectionDescription>& result);
}
#endif