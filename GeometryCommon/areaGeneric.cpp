#include "areaGeneric.h"
namespace geometry
{
	void areaSpecificArc(intersectionDescription& current, intersectionDescription& next, float& polygonArea, float& arcArea)
	{
		const Shape* object;
		if(current.objects.first == next.objects.first && current.objects.second == next.objects.second)
		{
			if(current.objects.first->getRadius() > current.objects.second->getRadius())
			{
				object = current.objects.first;
			}
			else object = current.objects.second;
		}
		else
		{
			if(current.objects.first == next.objects.first)
			{
				object = current.objects.first;
			}
			else if(current.objects.second == next.objects.second)
			{
				object = current.objects.second;
			}
			else if(current.objects.first == next.objects.second)
			{
				object = current.objects.first;
			}
			else
			{
				object = current.objects.second;
			}
		}
		cml::vector2f startPoint = current.point;
		cml::vector2f endPoint = next.point;

		float height = std::fabs((startPoint - endPoint)[1]);
		float startAbsX = std::fabs(startPoint[0]);
		float endAbsX = std::fabs(endPoint[0]);
		float minAbsX = std::min(startAbsX, endAbsX);
		float maxAbsX = std::max(startAbsX, endAbsX);
		float relevantArea = minAbsX * height + 0.5f * height * (maxAbsX - minAbsX);
		if((startPoint - endPoint)[1] < 0)
		{
			polygonArea += relevantArea;
		}
		else 
		{
			polygonArea -= relevantArea;
		}

		float radius = object->getRadius();
		if(radius < std::numeric_limits<float>::infinity())
		{
			float segmentLengthSquared = (startPoint - endPoint).length_squared();
			//it seems that floating point issues can make the segment length squared too big. This leads to calculating acos(x) where abs(x) > 1, which is an error. 
			segmentLengthSquared = std::min(segmentLengthSquared, 4*radius*radius);
			//area of a circular arc. Complicated by the fact that the "arc" could have more tha n180 degrees. In which case we split the angle into 4 parts, and multiply the 
			//area by four at the end.
			const Circle* circle = static_cast<const Circle*>(object);
			cml::vector2f firstDiff = startPoint - circle->pos;
			cml::vector2f secondDiff = endPoint - circle->pos;
			
			float firstAngle = atan2(firstDiff[1], firstDiff[0]);
			float secondAngle = atan2(secondDiff[1], secondDiff[0]);
			if(secondAngle < firstAngle) secondAngle += (float)(2*M_PI);
			//angle is the circular arc actually spanned, naiveAngle is the angle if we assume that the angle is less than 180 degrees
			float angle = fabs(firstAngle - secondAngle);
			if(angle > (float)2*M_PI) angle -= (float)(2*M_PI);
			if(angle < 0.0f) angle += (float)(2*M_PI);

			float naiveAngle = acos((segmentLengthSquared - 2 * radius*radius) / (- 2 *radius * radius));
			if(angle > M_PI)
			{
				arcArea += (float)(M_PI * radius * radius - 0.5f * (naiveAngle - sin(naiveAngle)) * radius* radius);
			}
			else
			{
				arcArea += (float)(0.5f * (naiveAngle - sin(naiveAngle)) * radius* radius);
			}
		}
	}
	float area(std::vector<intersectionDescription>& points)
	{
		if(points.size() == 0) return 0;
		float polygonArea = 0;
		float arcArea = 0;
		for(std::size_t i = 1; i < points.size(); i++)
		{
			intersectionDescription current = points[i-1];
			intersectionDescription next = points[i];
			areaSpecificArc(current, next, polygonArea, arcArea);
		}
		areaSpecificArc(*points.rbegin(), *points.begin(), polygonArea, arcArea);
#ifndef NDEBUG
		if(polygonArea < 0 && points.size() > 2) throw std::runtime_error("Internal error");
#endif
		return arcArea + polygonArea;
	}
}
