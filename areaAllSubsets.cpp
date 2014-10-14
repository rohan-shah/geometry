#include "areaAllSubsets.h"
#include "powerSetIterator.hpp"
#include "intersectionOutline.h"
#include "areaGeneric.h"
namespace geometry
{
	float areaOfUnionAllSubsets(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles)
	{
		allSubsetsIterator<Circle, SemiCircle> iter(circles, semicircles);
		std::vector<Circle> circleSubset;
		std::vector<SemiCircle> semicircleSubset;
		float totalArea = 0;
		while(!iter.done())
		{
			iter.extract(circleSubset, semicircleSubset);

			std::vector<intersectionDescription> containedInAll;
			intersectionOutline(circleSubset, boundingLines, semicircleSubset, containedInAll);

			if(containedInAll.size() == 0)
			{
				iter++;
				continue;
			}
#ifndef NDEBUG
			if(containedInAll.size() == 1) throw std::runtime_error("Cannot have a single intersection point");
#endif
			if(containedInAll.size() > 0)
			{
				float thisSubsetArea = area(containedInAll);
				if((circleSubset.size() + semicircleSubset.size()) % 2)
				{
					totalArea += thisSubsetArea;
				}
				else
				{
					totalArea -= thisSubsetArea;
				}
			}
			iter++;
		}
		return totalArea;
	}
}