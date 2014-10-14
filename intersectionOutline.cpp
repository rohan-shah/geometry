#include "intersectionOutline.h"
#include "pairwiseIntersections.h"
namespace geometry
{
	void intersectionOutlineOneCircle(const Circle* circle, const std::vector<Line>& boundingLines, std::vector<intersectionDescription>& result)
	{
		float radius = circle->radius;
		result.clear();
		intersectionsOneCircle(circle, boundingLines, result);

		cml::vector2f internalPoint(0,0);
		for(int i = 0; i < result.size(); i++)
		{
			internalPoint += result[i].point;
		}
		internalPoint /= (float)result.size();
		std::sort(result.begin(), result.end(), intersectionDescription::angleSorter(internalPoint));

		if(result.size() == 2)
		{
			cml::vector2f centre = circle->pos;
			cml::vector2f initialDifference = centre - result[0].point;
			cml::vector2f finalDifference = centre - result[1].point;
			float initialAngle = atan2(initialDifference[1], initialDifference[0]);
			float finalAngle = atan2(finalDifference[1], finalDifference[0]);
			float meanAngle = (initialAngle + finalAngle)/2;
			cml::vector2f between[2] = {cml::vector2f(cos(meanAngle), sin(meanAngle))*radius + centre,  -cml::vector2f(cos(meanAngle), sin(meanAngle))*radius + centre};

			bool keep = true;
			for(int j = 0; j < 2; j++)
			{
				bool keep = true;
				for(std::vector<Line>::const_iterator k = boundingLines.begin(); k != boundingLines.end(); k++)
				{
					if(!k->correctSide(between[j]))
					{
						keep = false;
					}
				}
				if(keep)
				{
					result.push_back(intersectionDescription(between[j], intersectionDescription::intersectingObjects(circle, circle)));
				}
			}
			std::sort(result.begin(), result.end(), intersectionDescription::angleSorter(internalPoint));
		}
	}
	void intersectionOutline(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles, std::vector<intersectionDescription>& result)
	{
		result.clear();
		std::vector<intersectionDescription> allIntersections;
		pairwiseIntersections(circles, boundingLines, semicircles, allIntersections);

		//discard points that aren't contained in all the circles. 
		for(std::vector<intersectionDescription>::const_iterator i = allIntersections.begin(); i != allIntersections.end(); i++)
		{
			for(std::vector<Circle>::const_iterator j = circles.begin(); j != circles.end(); j++)
			{
				if(&*j != i->objects.first && &*j != i->objects.second && (i->point - j->pos).length_squared() >= j->radius * j->radius)
				{
					goto notContainedInAll;
				}
			}
			for(std::vector<SemiCircle>::const_iterator j = semicircles.begin(); j != semicircles.end(); j++)
			{
				if(static_cast<const Shape*>(&(j->circle)) != i->objects.first && static_cast<const Shape*>(&(j->circle)) != i->objects.second && static_cast<const Shape*>(&(j->straightLine)) != i->objects.first && static_cast<const Shape*>(&(j->straightLine)) != i->objects.second 
					&& ((i->point - j->circle.pos).length_squared() >= j->circle.radius * j->circle.radius || !j->straightLine.correctSide(i->point)))
				{
					goto notContainedInAll;
				}
			}
			result.push_back(*i);
notContainedInAll:
			;
		}
		//get out an internal point, and use this to order the intersections points by angle relative to a fixed internal point. 
		cml::vector2f internalPoint(0,0);
		for(int i = 0; i < result.size(); i++)
		{
			internalPoint += result[i].point;
		}
		internalPoint /= (float)result.size();
		std::sort(result.begin(), result.end(), intersectionDescription::angleSorter(internalPoint));
		//There's a problem if there are only two intersection points, in which case the counter-clockwise ordering could concievably be the same as the clockwise ordering. So we add in
		//extra points to make it clear which way round we want to go. 
		if(result.size() == 2)
		{
			std::vector<const Circle*> resultCircles;
			for(int i = 0; i < 2; i++)
			{
				if(result[i].objects.first->getRadius() < std::numeric_limits<float>::infinity())
				{
					resultCircles.push_back(static_cast<const Circle*>(result[i].objects.first));
				}
				if(result[i].objects.second->getRadius() < std::numeric_limits<float>::infinity())
				{
					resultCircles.push_back(static_cast<const Circle*>(result[i].objects.second));
				}
			}
			std::sort(resultCircles.begin(), resultCircles.end());
			resultCircles.erase(std::unique(resultCircles.begin(), resultCircles.end()), resultCircles.end());
			for(std::vector<const Circle*>::const_iterator i = resultCircles.begin(); i != resultCircles.end(); i++)
			{
				cml::vector2f centre = (*i)->pos;
				cml::vector2f initialDifference = centre - result[0].point;
				cml::vector2f finalDifference = centre - result[1].point;
				float initialAngle = atan2(initialDifference[1], initialDifference[0]);
				float finalAngle = atan2(finalDifference[1], finalDifference[0]);
				float meanAngle = (initialAngle + finalAngle)/2;
				cml::vector2f between[2] = {cml::vector2f(cos(meanAngle), sin(meanAngle))*(*i)->radius + centre,  -cml::vector2f(cos(meanAngle), sin(meanAngle))*(*i)->radius + centre};

				bool keep = true;
				for(int j = 0; j < 2; j++)
				{
					bool keep = true;
					for(std::vector<Circle>::const_iterator k = circles.begin(); k != circles.end(); k++)
					{
						if(&(*k) != *i && (between[j] - k->pos).length_squared() > k->radius * k->radius) keep = false;
					}
					for(std::vector<Line>::const_iterator k = boundingLines.begin(); k != boundingLines.end(); k++)
					{
						if(!k->correctSide(between[j]))
						{
							keep = false;
						}
					}
					for(std::vector<SemiCircle>::const_iterator k = semicircles.begin(); k != semicircles.end(); k++)
					{
						if((*i != &(k->circle) && (between[j] - k->circle.pos).length_squared() > k->circle.radius * k->circle.radius) || !k->straightLine.correctSide(between[j]))
						{
							keep = false;
						}
					}
					if(keep)
					{
						result.push_back(intersectionDescription(between[j], intersectionDescription::intersectingObjects(*i, *i)));
					}
				}
			}
			cml::vector2f internalPoint(0,0);
			for(int i = 0; i < result.size(); i++)
			{
				internalPoint += result[i].point;
			}
			internalPoint /= (float)result.size();
			std::sort(result.begin(), result.end(), intersectionDescription::angleSorter(internalPoint));
		}
		//even if there's at least three points, if there are two intersections of the same pair of circles then it can still be unclear which one to follow around. So 
		//we add in extra points in between, listed as self-intersections. 
		else if(circles.size() + semicircles.size() > 1)
		{
			std::vector<intersectionDescription> additional;
			for(std::vector<intersectionDescription>::iterator current = result.begin(); current!=result.end(); current++)
			{
				std::vector<intersectionDescription>::iterator next;
				if(current + 1 == result.end())
				{
					next = result.begin();
				}
				else
				{
					next = current+1;
				}
				if(next->objects.first == current->objects.first && next->objects.second == current->objects.second && current->objects.first->getRadius() < std::numeric_limits<float>::infinity() && current->objects.second->getRadius() < std::numeric_limits<float>::infinity())
				{
					const Circle* firstCircle = static_cast<const Circle*>(current->objects.first);
					const Circle* secondCircle = static_cast<const Circle*>(current->objects.second);
					cml::vector2f firstDiff = current->point - firstCircle->pos;
					cml::vector2f secondDiff = next->point - firstCircle->pos;

					float finalAngle = atan2(firstDiff[1], firstDiff[0]);
					float initialAngle = atan2(secondDiff[1], secondDiff[0]);
					if(initialAngle < finalAngle) finalAngle+=(float)(2*M_PI);

					float meanAngle = (initialAngle + finalAngle)/2;
					cml::vector2f meanPoint = cml::vector2f(cos(meanAngle), sin(meanAngle))*firstCircle->getRadius() + firstCircle->pos;

					if((secondCircle->pos - meanPoint).length_squared() <= secondCircle->radius * secondCircle->radius)
					{
						additional.push_back(intersectionDescription(meanPoint, intersectionDescription::intersectingObjects(firstCircle, firstCircle)));
					}
					else
					{
						cml::vector2f firstDiff = current->point - secondCircle->pos;
						cml::vector2f secondDiff = next->point - secondCircle->pos;

						float finalAngle = atan2(firstDiff[1], firstDiff[0]);
						float initialAngle = atan2(secondDiff[1], secondDiff[0]);
						if(initialAngle < finalAngle) finalAngle+=(float)(2*M_PI);

						float meanAngle = (initialAngle + finalAngle)/2;
						cml::vector2f meanPoint = cml::vector2f(cos(meanAngle), sin(meanAngle))*secondCircle->getRadius() + secondCircle->pos;
						additional.push_back(intersectionDescription(meanPoint, intersectionDescription::intersectingObjects(secondCircle, secondCircle)));
					}
				}
			}
			//if we added anything, resort
			if(additional.size() > 0)
			{
				result.insert(result.end(), additional.begin(), additional.end());
				cml::vector2f internalPoint(0,0);
				for(int i = 0; i < result.size(); i++)
				{
					internalPoint += result[i].point;
				}
				internalPoint /= (float)result.size();
				std::sort(result.begin(), result.end(), intersectionDescription::angleSorter(internalPoint));
			}
		}
	}
}