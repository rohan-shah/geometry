#include "voronoiViewer.h"
#include <limits>
#include <QEvent>
#include "powerSetIterator.hpp"
#include <sstream>
#include <QGraphicsSimpleTextItem>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Regular_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Regular_triangulation_vertex_base_2.h>
#include <CGAL/Triangulation_face_base_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Regular_triangulation_face_base_2.h>
#include <CGAL/Regular_triangulation_euclidean_traits_2.h>
#include <CGAL/Regular_triangulation_filtered_traits_2.h>
#include <CGAL/Regular_triangulation_adaptation_traits_2.h>
#include <CGAL/Regular_triangulation_adaptation_policies_2.h>
#include "intersectionOutline.h"
#include "areaAllSubsets.h"
namespace poissonOrdered
{
	voronoiViewer::voronoiViewer(const std::vector<geometry::Circle>& circles, const std::vector<geometry::Line>& lines, const std::vector<geometry::SemiCircle>& semicircles)
		:circles(circles), lines(lines), semicircles(semicircles)
	{
		graphicsScene = new QGraphicsScene();
		graphicsScene->installEventFilter(this);
		graphicsScene->setItemIndexMethod(QGraphicsScene::NoIndex);

		graphicsView = new QGraphicsView(graphicsScene);
		graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		graphicsView->viewport()->installEventFilter(this);
		graphicsView->installEventFilter(this);

		setCentralWidget(graphicsView);

		minX = std::numeric_limits<float>::infinity(), maxX = -std::numeric_limits<float>::infinity();
		minY = minX, maxY = maxX;
		if(circles.size() + semicircles.size() == 0)
		{
			minX = 0;
			maxX = 1;
			minY = 0;
			maxY = 1;
		}
		for(std::vector<geometry::Circle>::const_iterator i = circles.begin(); i!=circles.end();i++)
		{
			if(i->pos[0] - i->radius < minX)
			{
				minX = i->pos[0] - i->radius;
			}
			if(i->pos[1] - i->radius < minY)
			{
				minY = i->pos[1] - i->radius;
			}

			if(i->pos[0] + i->radius > maxX)
			{
				maxX = i->pos[0] + i->radius;
			}
			if(i->pos[1] + i->radius > maxY)
			{
				maxY = i->pos[1] + i->radius;
			}
		}
		for(std::vector<geometry::SemiCircle>::const_iterator i = semicircles.begin(); i!=semicircles.end();i++)
		{
			if(i->circle.pos[0] - i->circle.radius < minX)
			{
				minX = i->circle.pos[0] - i->circle.radius;
			}
			if(i->circle.pos[1] - i->circle.radius < minY)
			{
				minY = i->circle.pos[1] - i->circle.radius;
			}
			if(i->circle.pos[0] + i->circle.radius > maxX)
			{
				maxX = i->circle.pos[0] + i->circle.radius;
			}
			if(i->circle.pos[1] + i->circle.radius > maxY)
			{
				maxY = i->circle.pos[1] + i->circle.radius;
			}
		}
		QPen pen(QColor("black"));
		pen.setStyle(Qt::SolidLine);
		pen.setWidthF(0.01);

		QPen bluePen(QColor("blue"));
		bluePen.setWidthF(0.01);

		this->resize(this->width(), this->height());
		//draw circles
		for(std::vector<geometry::Circle>::const_iterator i = circles.begin(); i!=circles.end();i++)
		{
			graphicsScene->addEllipse(i->pos[0] - i->radius, i->pos[1] - i->radius, 2*i->radius, 2*i->radius, pen);
		}
		//draw semicircles
		for(std::vector<geometry::SemiCircle>::const_iterator i = semicircles.begin(); i != semicircles.end();i++)
		{
			cml::vector2f point = i->circle.pos;
			cml::vector2f line = i->straightLine.unitDirection;
			line /= line.length_squared();
			float radius = i->circle.radius;
			QPainterPath path;
			path.arcMoveTo(point[0]-radius, point[1]-radius, 2*radius, 2*radius, -atan2(line[1], line[0])*360/(2*M_PI));
			path.arcTo(point[0]-radius, point[1]-radius, 2*radius, 2*radius, -atan2(line[1], line[0])*360/(2*M_PI), -180);
			path.closeSubpath();
			graphicsScene->addPath(path, pen);
			path = QPainterPath();
			path.arcMoveTo(point[0]-radius, point[1]-radius, 2*radius, 2*radius, -atan2(line[1], line[0])*360/(2*M_PI)+180);
			path.arcTo(point[0]-radius, point[1]-radius, 2*radius, 2*radius, -atan2(line[1], line[0])*360/(2*M_PI)+180,-180);
			graphicsScene->addPath(path, bluePen);
		}
		geometry::Line boundaries[4] = {geometry::Line(cml::vector2f(minX, minY), cml::vector2f(0, 1)), geometry::Line(cml::vector2f(minX, minY), cml::vector2f(1, 0)), geometry::Line(cml::vector2f(maxX, maxY), cml::vector2f(-1, 0)), geometry::Line(cml::vector2f(maxX, maxY), cml::vector2f(0, -1))};

		//restrict line segments to window.

		for(std::vector<geometry::Line>::const_iterator i = lines.begin(); i != lines.end(); i++)
		{
			bool found;
			bool first = true, second = false;
			cml::vector2f firstPoint, secondPoint;

			cml::vector2f intersection = i->intersection(boundaries[0], found);
			if(found && intersection[0] >= minX && intersection[0] <= maxX && intersection[1] >= minY && intersection[1] <= maxY)
			{
				firstPoint = intersection;
				first = false;
			}

			intersection = i->intersection(boundaries[1], found);
			if(found && intersection[0] > minX && intersection[0] < maxX && intersection[1] >= minY && intersection[1] <= maxY)
			{
				if(first)
				{
					firstPoint = intersection;
					first = false;
				}
				else 
				{
					secondPoint = intersection;
					second = true;
					goto foundIntersections;
				}
			}

			intersection = i->intersection(boundaries[2], found);
			if(found && intersection[0] > minX && intersection[0] < maxX && intersection[1] >= minY && intersection[1] <= maxY)
			{
				if(first)
				{
					firstPoint = intersection;
					first = false;
				}
				else 
				{
					secondPoint = intersection;
					second = true;
					goto foundIntersections;
				}
			}

			intersection = i->intersection(boundaries[3], found);
			if(found && intersection[0] >= minX && intersection[0] <= maxX && intersection[1] >= minY && intersection[1] <= maxY)
			{
				if(first)
				{
					firstPoint = intersection;
					first = false;
				}
				else 
				{
					secondPoint = intersection;
					second = true;
					goto foundIntersections;
				}
			}
foundIntersections:
			if(second)
			{
				graphicsScene->addLine(firstPoint[0], firstPoint[1], secondPoint[0], secondPoint[1], pen);
			}
		}
		addVoronoiLines();
		//draw outline of intersection region
		std::vector<geometry::Circle> circlesSubset;
		std::vector<geometry::SemiCircle> semicircleSubset;

		if(circles.size() + semicircles.size() > 0)
		{
			QPen noPen;
			noPen.setStyle(Qt::NoPen);
			QBrush redBrush(QColor("red"));
			redBrush.setStyle(Qt::SolidPattern);
			geometry::zipIterator<geometry::Circle, geometry::SemiCircle> pairs(circles, semicircles, 1);
			while(!pairs.done())
			{
				pairs.extract(circlesSubset, semicircleSubset);
	
				std::vector<geometry::intersectionDescription> result;
				intersectionOutline(circlesSubset, lines, semicircleSubset, result);

				for(std::vector<geometry::intersectionDescription>::const_iterator i = result.begin(); i!= result.end(); i++)
				{
					graphicsScene->addEllipse(i->point[0]- 0.025, i->point[1] - 0.025, 0.05, 0.05, noPen, redBrush);
				}

				drawSubsetIntersectionOutline(result);
				pairs++;
			}
		}
	}
	void voronoiViewer::addVoronoiLines()
	{
		typedef CGAL::Simple_cartesian<float> K;
		typedef CGAL::Regular_triangulation_euclidean_traits_2<K> Traits;
		typedef CGAL::Regular_triangulation_face_base_2<Traits> Fb;
		typedef CGAL::Regular_triangulation_vertex_base_2<Traits, CGAL::Triangulation_vertex_base_with_info_2<std::pair<bool, const void*>, Traits> > Vb;
		typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
		typedef CGAL::Regular_triangulation_2<Traits,Tds> RG;
		typedef CGAL::Regular_triangulation_adaptation_traits_2<RG> AT;
		typedef CGAL::Regular_triangulation_caching_degeneracy_removal_policy_2<RG> AP;
		typedef CGAL::Voronoi_diagram_2<RG,AT,AP> VD;

		std::vector<geometry::SemiCircle> copiedSemicircles = semicircles;

		//the set of all circles contained in the voroni diagram
		std::vector<RG::Weighted_point> graphPoints;
		std::vector<std::pair<bool, const void*> > graphInfo;
		for(std::vector<geometry::Circle>::const_iterator i = circles.begin(); i != circles.end(); i++)
		{
			graphPoints.push_back(RG::Weighted_point(RG::Point(i->pos[0], i->pos[1]), i->radius * i->radius));
			graphInfo.push_back(std::make_pair(true, &*i));
		}
		for(std::vector<geometry::SemiCircle>::const_iterator i = copiedSemicircles.begin(); i != copiedSemicircles.end(); i++)
		{
			graphPoints.push_back(RG::Weighted_point(RG::Point(i->circle.pos[0], i->circle.pos[1]), i->circle.radius*i->circle.radius));
			graphInfo.push_back(std::make_pair(false, &*i));
		}
		RG rg;
		rg.insert(boost::make_zip_iterator(boost::make_tuple(graphPoints.begin(), graphInfo.begin())), boost::make_zip_iterator(boost::make_tuple(graphPoints.end(), graphInfo.end())));
		VD vd(rg, true);
		//the cell boundaries. These ones are common to all cells
		std::vector<geometry::Line> cellBoundaries;
		cellBoundaries.insert(cellBoundaries.end(), lines.begin(), lines.end());

		//the outline of the contents of a voroni cell
		std::vector<geometry::intersectionDescription> outline;

		//the circle contained in the current voroni cell
		std::vector<geometry::Circle> currentCircle;
		
		QPen blackPen(QColor("black"));
		blackPen.setWidthF(0.01);
		QPen bluePen(QColor("blue"));
		bluePen.setWidthF(0.01);

		for(VD::Face_iterator i = vd.faces_begin(); i != vd.faces_end(); i++)
		{
			//get out current circle
			const geometry::Circle* currentCirclePtr;
			const void* currentObject;
			bool isCircle;
			boost::tie(isCircle, currentObject) = i->dual()->info();
			if(isCircle)
			{
				currentCirclePtr = static_cast<const geometry::Circle*>(currentObject);
			}
			else 
			{
				currentCirclePtr = &static_cast<const geometry::SemiCircle*>(currentObject)->circle;
			}

			//put current circle into vector
			currentCircle.clear();
			currentCircle.push_back(*currentCirclePtr);

			//ensure we only have the global cell boundaries currently set
			cellBoundaries.resize(lines.size());
			//set up cell boundaries
			if(vd.number_of_faces() > 1)
			{
				VD::Ccb_halfedge_circulator faceEdgesStart = i->ccb();
				VD::Ccb_halfedge_circulator faceCurrentEdge = faceEdgesStart;
				do
				{
					cml::vector2f pos, difference;
					CGAL::Object o = vd.dual().dual(faceCurrentEdge->dual());
					Traits::Ray_2 r;
					Traits::Segment_2 s;
					Traits::Line_2 l2;
					if(CGAL::assign(r,o))
					{
						pos = cml::vector2f(r.source().x(), r.source().y());
						difference = cml::vector2f(r.direction().vector().x(), r.direction().vector().y());
						difference.normalize();

						cml::vector2f start = pos;
						cml::vector2f end = pos + 5 * difference;
						graphicsScene->addLine(start[0], start[1], end[0], end[1], blackPen);
					}
					else if(CGAL::assign(s, o))
					{
						pos = cml::vector2f(s.source().x(), s.source().y());
						cml::vector2f end(s.target().x(), s.target().y());
						graphicsScene->addLine(pos[0], pos[1], end[0], end[1], blackPen);
					}
					else if(CGAL::assign(l2, o))
					{
						pos = cml::vector2f(l2.point().x(), l2.point().y());
						difference = cml::vector2f(l2.direction().vector().x(), l2.direction().vector().y());
						difference.normalize();
						cml::vector2f start = pos + 5 * difference;
						cml::vector2f end = pos - 5 * difference;
						graphicsScene->addLine(pos[0], pos[1], end[0], end[1], blackPen);
					}
					else throw std::runtime_error("");
				}
				while(++faceCurrentEdge != faceEdgesStart);
			}
		}
		graphicsView->fitInView(graphicsScene->itemsBoundingRect());
	}
	void voronoiViewer::drawSubsetIntersectionOutline(std::vector<geometry::intersectionDescription>& outline)
	{
		QPen noPen;
		noPen.setStyle(Qt::NoPen);

		QPen redPen(QColor("red"));
		redPen.setStyle(Qt::SolidLine);
		redPen.setWidthF(0.01);

		QBrush redBrush(QColor("red"));
		redBrush.setStyle(Qt::SolidPattern);

		QBrush noBrush;
		noBrush.setStyle(Qt::NoBrush);

		if(outline.size() == 0) return; 

		//Average all the points to get something in the centre.
		cml::vector2f internalPoint(0, 0);
		for(std::vector<geometry::intersectionDescription>::const_iterator i = outline.begin(); i!= outline.end(); i++)
		{
			internalPoint += i->point;
		}
		internalPoint /= outline.size();

		std::sort(outline.begin(), outline.end(), geometry::intersectionDescription::angleSorter(internalPoint));
		//Do we have only two objects, or more?
		std::vector<const geometry::Shape*> shapes;
		for(int i = 0; i < outline.size(); i++) 
		{
			shapes.push_back(outline[i].objects.first);
			shapes.push_back(outline[i].objects.second);
		}
		std::sort(shapes.begin(), shapes.end());
		shapes.erase(std::unique(shapes.begin(), shapes.end()), shapes.end());

		for(std::vector<geometry::intersectionDescription>::const_iterator i = outline.begin(); outline.size() > 0 && i+1!= outline.end(); i++)
		{
			geometry::intersectionDescription current = *i;
			geometry::intersectionDescription next = *(i+1);
			addOutline(current, next, redPen, noBrush);
		}
		if(outline.size() > 0)
		{
			geometry::intersectionDescription current = *outline.rbegin();
			geometry::intersectionDescription next = *outline.begin();
			addOutline(current, next, redPen, noBrush);
		}
	}
	void voronoiViewer::addOutline(geometry::intersectionDescription& current, geometry::intersectionDescription& next, QPen& redPen, QBrush& noBrush)
	{
			const geometry::Shape* object;
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
			QPainterPath path;
			float radius = object->getRadius();
			if(radius == std::numeric_limits<float>::infinity())
			{
				path.moveTo(current.point[0], current.point[1]);
				path.lineTo(next.point[0], next.point[1]);
			}
			else
			{
				const geometry::Circle* circle = static_cast<const geometry::Circle*>(object);
				
				cml::vector2f centralPoint = circle->pos;
				cml::vector2f initialDiff = current.point - centralPoint;
				cml::vector2f finalDiff = next.point - centralPoint;
				
				float initialAngle = atan2(initialDiff[1], initialDiff[0]) * (360 / (2*M_PI));
				float finalAngle = atan2(finalDiff[1], finalDiff[0]) * (360 / (2*M_PI));
				if(finalAngle < initialAngle) finalAngle+=360;

				path.moveTo(current.point[0], current.point[1]);
				path.arcTo(centralPoint[0] - radius, centralPoint[1] - radius, radius*2, radius*2, -initialAngle, -finalAngle + initialAngle);
			}
			graphicsScene->addPath(path, redPen, noBrush);
	}
	voronoiViewer::~voronoiViewer()
	{
		delete graphicsView;
		delete graphicsScene;
	}
	bool voronoiViewer::eventFilter(QObject* object, QEvent *event)
	{
		if(object == graphicsView && event->type() == QEvent::Resize)
		{
			//graphicsView->fitInView(minX - abs(minX)*0.01, minY - abs(minX)*0.01, (maxX + abs(maxX) * 0.01) - (minX - abs(minX) * 0.01), (maxY + abs(maxY) * 0.01) - (minY - abs(minY)*0.01), Qt::KeepAspectRatio);
			graphicsView->fitInView(graphicsScene->itemsBoundingRect());
			return false;
		}
		return false;
	}
	bool voronoiViewer::event(QEvent* event)
	{
		if(event->type() == QEvent::WindowStateChange)
		{
			QMainWindow::event(event);
			//graphicsView->fitInView(minX - abs(minX)*0.01, minY - abs(minX)*0.01, (maxX + abs(maxX) * 0.01) - (minX - abs(minX) * 0.01), (maxY + abs(maxY) * 0.01) - (minY - abs(minY)*0.01), Qt::KeepAspectRatio);
			graphicsView->fitInView(graphicsScene->itemsBoundingRect());
			return true;
		}
		return false;
	}
}