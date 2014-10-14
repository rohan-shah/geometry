#ifndef AREA_ALL_SUBSETS_HEADER_GUARD
#define AREA_ALL_SUBSETS_HEADER_GUARD
#include <vector>
#include "Shapes.h"
namespace geometry
{
	float areaOfUnionAllSubsets(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles);
}
#endif