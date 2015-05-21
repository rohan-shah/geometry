#ifndef INTERSECTION_OUTLINE_HEADER_GUARD
#define INTERSECTION_OUTLINE_HEADER_GUARD
#include <vector>
#include "Shapes.h"
namespace geometry
{
	/*
	 * Do some extra processing to make the set of intersection points suitable for drawing the outline. Specifically, exclude intersection points that aren't included in all
	 * the circles. Also, if after we've ordered the points there's still some ambiguity about which circle arcs we follow, add in self-intersection points to make it clear. 
	 */
	void intersectionOutline(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles, std::vector<intersectionDescription>& result);
	void intersectionOutlineOneCircle(const Circle* circle, const std::vector<Line>& boundingLines, std::vector<intersectionDescription>& result);
}
#endif