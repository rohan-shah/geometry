#ifndef PAIRWISE_INTERSECTIONS_HEADER_GUARD
#define PAIRWISE_INTERSECTIONS_HEADER_GUARD
#include "Shapes.h"
namespace geometry
{
	/*
	 * Roughly speaking, get the intersections of all of these circles, in the convex region bounded by the straight line segments. There are a number of exceptions to this interpretation:
	 * -If any one circle is contained within another, four equally spaced points on the contained circle will be output. These points will be listed as self-intersections
	 * -If there is only one circle at all, four extra points will be added on the circle. These points will be listed as self intersections. These points might lie outside the 
	 * convex region and therefore be removed, so between 0 and 4 self-intersections will be added. 
	 *
	 * Any intersections that lie outside the convex region defined by the straight lines are ignored.
	 * The convention is that a point is on the interior side of a straight line segment if the minimum distance vector is a POSITIVE multiple of (b, -a).
	 * So for a line in direction (0, 1), the interior is to the RIGHT
	 * For a line segment in direction (1, 0), the interior is ABOVE
	 * For a line segment in direction (0, -1), the interior is to the LEFT
	 * For a line segment in direction (-1, 0), the interior is to the BELOW
	 */
	void pairwiseIntersections(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles, std::vector<intersectionDescription>& result);
	void intersectionsOneCircle(const Circle* circle, const std::vector<Line>& boundingLines, std::vector<intersectionDescription>& result);
}
#endif