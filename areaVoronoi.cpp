#include "areaVoronoi.h"
#include "intersectionOutline.h"
#include "areaGeneric.h"
#include "primitiveIntersections.hpp"
namespace geometry
{
	namespace areaOfUnionVoronoiImpl
	{
		float areaOfUnionVoronoiNoSemicircles(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, VD& vd, bool useExistingVD)
		{
			if(circles.size() == 0) return 0;

			float totalArea = 0;
			if(!useExistingVD)
			{
				//the set of all circles contained in the voroni diagram
				std::vector<RG::Weighted_point> graphPoints;
				std::vector<const Circle*> graphInfo;
		
	
				for(std::vector<Circle>::const_iterator i = circles.begin(); i != circles.end(); i++)
				{
					graphPoints.push_back(RG::Weighted_point(RG::Point(i->pos[0], i->pos[1]), i->radius * i->radius));
					graphInfo.push_back(&*i);
				}
				RG rg;
				rg.insert(boost::make_zip_iterator(boost::make_tuple(graphPoints.begin(), graphInfo.begin())), boost::make_zip_iterator(boost::make_tuple(graphPoints.end(), graphInfo.end())));
				VD newVD(rg, true);
				newVD.swap(vd);
			}
			const RG& rgRef = vd.dual();
			//the cell boundaries. These ones are common to all cells
			std::vector<Line> cellBoundaries;
			cellBoundaries.insert(cellBoundaries.end(), boundingLines.begin(), boundingLines.end());

			//the outline of the contents of a voroni cell
			std::vector<intersectionDescription> outline;

			//the circle contained in the current voroni cell
			std::vector<Circle> currentCircle;
		
			//work out if it has more than 1 face or not
			int countFaces = 0;
			for(VD::Face_iterator i = vd.faces_begin(); i != vd.faces_end(); i++)
			{
				countFaces++;
				if(countFaces >= 2) break;
			}
			bool hasMultipleFaces = countFaces > 1;
			for(VD::Face_iterator i = vd.faces_begin(); i != vd.faces_end(); i++)
			{
				if(i->dual()->is_hidden()) continue;
				//get out current circle
				const Circle* currentCirclePtr;
				currentCirclePtr = i->dual()->info();

				//put current circle into vector
				currentCircle.clear();
				currentCircle.push_back(*currentCirclePtr);

				//ensure we only have the global cell boundaries currently set
				cellBoundaries.resize(boundingLines.size());
				//set up cell boundaries
				if(hasMultipleFaces)
				{
					VD::Ccb_halfedge_circulator faceEdgesStart = i->ccb();
					VD::Ccb_halfedge_circulator faceCurrentEdge = faceEdgesStart;
					do
					{
						cml::vector2f pos, difference;
						const geometry::Circle* otherCirclePtr = faceCurrentEdge->opposite()->face()->dual()->info();;
						/* Copied from Regular_Triangulation.h, 
								Regular_triangulation_2<Gt,Tds>::dual(const Edge &e) const
						*/
						RG::Edge e = faceCurrentEdge->dual();
						CGAL_triangulation_precondition (! rgRef.is_infinite(e));
						if(rgRef.dimension() == 1)
						{
							const RG::Weighted_point& p = (e.first)->vertex(rgRef.cw(e.second))->point();
							const RG::Weighted_point& q = (e.first)->vertex(rgRef.ccw(e.second))->point();
							Traits::Line_2 l = rgRef.geom_traits().construct_radical_axis_2_object()(p,q);
							
							pos = cml::vector2f((float)l.point().x(), (float)l.point().y());
							difference = cml::vector2f((float)l.direction().vector().x(), (float)l.direction().vector().y());
						}
						else if( (! rgRef.is_infinite(e.first)) &&
							  (! rgRef.is_infinite(e.first->neighbor(e.second))) ) {
							/*RG::Segment s = rgRef.geom_traits().construct_segment_2_object()
							  (rgRef.dual(e.first),rgRef.dual(e.first->neighbor(e.second)));
								//if the norm of one of the points is huge, pick the other one.
								pos = cml::vector2f(s.source().x(), s.source().y());
								if(pos.length_squared() > 1e6)
								{
									pos = cml::vector2f((float)s.target().x(), (float)s.target().y());
								}
								difference = cml::vector2f((float)s.direction().vector().x(), (float)s.direction().vector().y());*/
								//temp2 - temp1
								RG::Point firstPoint = rgRef.dual(e.first);
								RG::Point secondPoint = rgRef.dual(e.first->neighbor(e.second));
								cml::vector2f firstPoint_(firstPoint[0], firstPoint[1]);
								cml::vector2f secondPoint_(secondPoint[0], secondPoint[1]);
								/*if((difference - (secondPoint_ - firstPoint_)).length_squared() > 1e-4)
								{
									throw std::runtime_error("");
								}
								if(firstPoint_.length_squared() > 1e6)
								{
									if((pos - secondPoint_).length_squared() > 1e-4)
									{
										throw std::runtime_error("");
									}
								}
								else
								{
									if((pos - temp1_).length_squared() > 1e-4)
									{
										throw std::runtime_error("");
									}
								}*/
								difference = secondPoint_ - firstPoint_;
								if(firstPoint_.length_squared() > 1e6)
								{
									pos = secondPoint_;
								}
								else pos = firstPoint_;

						  } else
						{

						  // one of the adjacent faces is infinite
						  RG::Face_handle f; int i;
						  if ( rgRef.is_infinite(e.first)) {
							f=e.first->neighbor(e.second); i=f->index(e.first);
						  } 
						  else {
							f=e.first; i=e.second;
						  }
						  const RG::Weighted_point& p = f->vertex( rgRef.cw(i))->point();
						  const RG::Weighted_point& q = f->vertex( rgRef.ccw(i))->point();
						  RG::Geom_traits::Line_2 l  = rgRef.geom_traits().construct_radical_axis_2_object()(p,q);
						  Traits::Ray_2 r = rgRef.geom_traits().construct_ray_2_object()(rgRef.dual(f), l);
						  pos = cml::vector2f((float)r.source().x(), (float)r.source().y());
						  difference = cml::vector2f((float)r.direction().vector().x(), (float)r.direction().vector().y());
						}
						//Original code follows:
						/*
						CGAL::Object o = rgRef.dual(faceCurrentEdge->dual());
						if(CGAL::assign(r,o))
						{
							pos = cml::vector2f((float)r.source().x(), (float)r.source().y());
							difference = cml::vector2f((float)r.direction().vector().x(), (float)r.direction().vector().y());
						}
						else if(CGAL::assign(s, o))
						{
							//if the norm of one of the points is huge, pick the other one.
							pos = cml::vector2f(s.source().x(), s.source().y());
							if(pos.length_squared() > 1e6)
							{
								pos = cml::vector2f((float)s.target().x(), (float)s.target().y());
							}
							difference = cml::vector2f((float)s.direction().vector().x(), (float)s.direction().vector().y());
						}
						else if(CGAL::assign(l2, o))
						{
							pos = cml::vector2f((float)l2.point().x(), (float)l2.point().y());
							difference = cml::vector2f((float)l2.direction().vector().x(), (float)l2.direction().vector().y());
						}
						else throw std::runtime_error("");*/

						difference.normalize();
						geometry::Line l(pos, difference);
						//if both centres are on the same side, it's a little tricky to tell which way the line should be going. 
						if(l.correctSide(currentCirclePtr->pos) == l.correctSide(otherCirclePtr->pos))
						{
							if(currentCirclePtr->radius > otherCirclePtr->radius)
							{
								if(!l.correctSide(currentCirclePtr->pos))
								{
									l.unitDirection *= -1;
								}
							}
							else
							{
								if(l.correctSide(currentCirclePtr->pos))
								{
									l.unitDirection *= -1;
								}
							}
						}
						//if centres are on opposite sides, then it's clear that the centre of this object should be on the "internal" side. 
						else
						{
							if(!l.correctSide(currentCirclePtr->pos))
							{
								l.unitDirection *= -1;
							}
						}
						cellBoundaries.push_back(l);
					}
					while(++faceCurrentEdge != faceEdgesStart);
				}
				outline.clear();
				intersectionOutlineOneCircle(currentCirclePtr, cellBoundaries, outline);
				float newArea = area(outline);
				totalArea += newArea;
			}
			return totalArea;
		}
	}
	float areaOfUnionVoronoiNoSemicircles(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines)
	{
		areaOfUnionVoronoiImpl::VD vd;
		return areaOfUnionVoronoiImpl::areaOfUnionVoronoiNoSemicircles(circles, boundingLines, vd, false);
	}
	float areaOfUnionVoronoiMutable(std::vector<Circle>& circles, std::vector<Line>& boundingLines, std::vector<SemiCircle>& semicircles)
	{
		if(semicircles.size() == 0) return areaOfUnionVoronoiNoSemicircles(circles, boundingLines);
		const SemiCircle last = *semicircles.rbegin();
		semicircles.pop_back();

		circles.push_back(last.circle);
		boundingLines.push_back(last.straightLine);
		
		float first = areaOfUnionVoronoiMutable(circles, boundingLines, semicircles);
		
		circles.pop_back();
		boundingLines.rbegin()->unitDirection *= -1;
		float second = areaOfUnionVoronoiMutable(circles, boundingLines, semicircles);

		boundingLines.pop_back();
		semicircles.push_back(last);
		return first + second;
	}
	float areaOfUnionVoronoi(const std::vector<Circle>& circles, const std::vector<Line>& boundingLines, const std::vector<SemiCircle>& semicircles)
	{
		if(semicircles.size() == 0) return areaOfUnionVoronoiNoSemicircles(circles, boundingLines);
		std::vector<Circle> copiedCircles = circles;
		std::vector<SemiCircle> copiedSemicircles = semicircles;
		std::vector<Line> copiedBoundingLines = boundingLines;

		return areaOfUnionVoronoiMutable(copiedCircles, copiedBoundingLines, copiedSemicircles);
	}
}