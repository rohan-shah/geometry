#include "pairwiseIntersections.h"
#include "primitiveIntersections.hpp"
namespace geometry
{
	void intersectionsOneCircle(const Circle* circle, const std::vector<Line>& boundingLines, std::vector<intersectionDescription>& result)
	{
		float radius = circle->radius;
		float radius2 = radius*radius;
		cml::vector2f pos = circle->pos;
		result.clear();

		for(std::vector<Line>::const_iterator j = boundingLines.begin(); j != boundingLines.end(); j++)
		{
			intersect(*circle, *j, result);
		}
		//add in line/line intersections
		for(std::vector<Line>::const_iterator i = boundingLines.begin(); i != boundingLines.end(); i++)
		{
			for(std::vector<Line>::const_iterator j = i+1; j != boundingLines.end(); j++)
			{
				bool found;
				cml::vector2f intersection = i->intersection(*j, found);
				if(found && (intersection - pos).length_squared() <= radius2) result.push_back(intersectionDescription(intersection, intersectionDescription::intersectingObjects(&(*i), &(*j))));
			}
		}
		if(result.size() == 0)
		{
			result.push_back(intersectionDescription(cml::vector2f(cos(0.0f), sin(0.0f))*radius + pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(circle), static_cast<const Shape*>(circle))));
			result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI/2), sin((float)M_PI/2))*radius + pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(circle), static_cast<const Shape*>(circle))));
			result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI), sin((float)M_PI))*radius + pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(circle), static_cast<const Shape*>(circle))));
			result.push_back(intersectionDescription(cml::vector2f(cos(3*(float)M_PI/2), sin(3*(float)M_PI/2))*radius + pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(circle), static_cast<const Shape*>(circle))));
		}
		//Discard intersections that fall outside the covex region
		for(std::vector<Line>::const_iterator j = boundingLines.begin(); j != boundingLines.end(); j++)
		{
			std::vector<intersectionDescription>::iterator resultIterator = result.begin();
			const Shape* currentLinePtr = static_cast<const Shape*>(&*j);
			while(resultIterator != result.end())
			{
				if(resultIterator->objects.first != currentLinePtr && resultIterator->objects.second != currentLinePtr && !j->correctSide(resultIterator->point))
				{
					resultIterator = result.erase(resultIterator);
				}
				else
				{
					resultIterator++;
				}
			}
		}
#ifndef NDEBUG
		if(result.size() == 1)
		{
			throw std::runtime_error("Internal error");
		}
#endif
	}
	void pairwiseIntersections(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles, std::vector<intersectionDescription>& result)
	{
		result.clear();
		std::size_t solidObjects = circles.size() + semicircles.size();
		if(solidObjects == 0)
		{
			throw std::runtime_error("Must have at least one circle to call intersectionPoints");
		}
		//add circle-circle intersections. 
		for(std::vector<Circle>::const_iterator i = circles.begin(); i != circles.end(); i++)
		{
			for(std::vector<Circle>::const_iterator j = i+1; j != circles.end(); j++)
			{
				intersect(*i, *j, result);	
			}
		}
		//Another special case if there is only one circle, add in four points on the boundary. Some of these may lie outside the convex region and later on be removed again. 
		if(solidObjects == 1)
		{
			if(circles.size() == 1)
			{
				result.push_back(intersectionDescription(cml::vector2f(cos(0.0f), sin(0.0f))*circles[0].radius + circles[0].pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&(circles[0])), static_cast<const Shape*>(&(circles[0])))));
				result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI/2), sin((float)M_PI/2))*circles[0].radius + circles[0].pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&(circles[0])), static_cast<const Shape*>(&(circles[0])))));
				result.push_back(intersectionDescription(cml::vector2f(cos((float)M_PI), sin((float)M_PI))*circles[0].radius + circles[0].pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&(circles[0])), static_cast<const Shape*>(&(circles[0])))));
				result.push_back(intersectionDescription(cml::vector2f(cos(3*(float)M_PI/2), sin(3*(float)M_PI/2))*circles[0].radius + circles[0].pos, intersectionDescription::intersectingObjects(static_cast<const Shape*>(&(circles[0])), static_cast<const Shape*>(&(circles[0])))));
			}
		}
		//Add in circle / line intersections
		for(std::vector<Circle>::const_iterator i = circles.begin(); i != circles.end(); i++)
		{
			for(std::vector<Line>::const_iterator j = boundingLines.begin(); j != boundingLines.end(); j++)
			{
				intersect(*i, *j, result);
			}
		}
		//add in line/line intersections
		for(std::vector<Line>::const_iterator i = boundingLines.begin(); i != boundingLines.end(); i++)
		{
			for(std::vector<Line>::const_iterator j = i+1; j != boundingLines.end(); j++)
			{
				bool found;
				cml::vector2f intersection = i->intersection(*j, found);
				if(found) result.push_back(intersectionDescription(intersection, intersectionDescription::intersectingObjects(&(*i), &(*j))));
			}
		}
		//add in semicircle / line intersections
		for(std::vector<SemiCircle>::const_iterator i = semicircles.begin(); i != semicircles.end(); i++)
		{
			//also semicircle self-intersections, where the linear bit hits the circle bit
			intersect(i->circle, i->straightLine, result);
			for(std::vector<Line>::const_iterator j = boundingLines.begin(); j != boundingLines.end(); j++)
			{
				intersect(*j, *i, result);
			}
		}
		//add in semicircle / semicircle intersections 
		for(std::vector<SemiCircle>::const_iterator i = semicircles.begin(); i != semicircles.end(); i++)
		{
			for(std::vector<SemiCircle>::const_iterator j = i+1; j != semicircles.end(); j++)
			{
				intersect(*j, *i, result);
			}
		}
		//add in semicircle / circle intersections 
		for(std::vector<SemiCircle>::const_iterator i = semicircles.begin(); i != semicircles.end(); i++)
		{
			for(std::vector<Circle>::const_iterator j = circles.begin(); j != circles.end(); j++)
			{
				intersect(*j, *i, result);
			}
		}
		//Discard intersections that fall outside the covex region
		for(std::vector<Line>::const_iterator j = boundingLines.begin(); j != boundingLines.end(); j++)
		{
			std::vector<intersectionDescription>::iterator resultIterator = result.begin();
			while(resultIterator != result.end())
			{
				if(resultIterator->objects.first != static_cast<const Shape*>(&*j) && resultIterator->objects.second != static_cast<const Shape*>(&*j) && !j->correctSide(resultIterator->point))
				{
					resultIterator = result.erase(resultIterator);
				}
				else
				{
					resultIterator++;
				}
			}
		}
		std::sort(result.begin(), result.end(), intersectionDescription::lexographicSorter());
		result.erase(std::unique(result.begin(), result.end()), result.end());
#ifndef NDEBUG
		if(result.size() == 1)
		{
			throw std::runtime_error("Internal error");
		}
#endif
	}
}