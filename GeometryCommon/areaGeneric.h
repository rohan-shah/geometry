#ifndef AREA_GENERIC_HEADER_GUARD
#define AREA_GENERIC_HEADER_GUARD
#include "Shapes.h"
namespace geometry
{
	void areaSpecificArc(intersectionDescription& current, intersectionDescription& next, float& polygonArea, float& arcArea);
	float area(std::vector<intersectionDescription>& points);
}
#endif