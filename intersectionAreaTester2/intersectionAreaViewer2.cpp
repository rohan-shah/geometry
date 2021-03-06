#include "intersectionAreaViewer2.h"
#include <limits>
#include <QEvent>
#include "powerSetIterator.hpp"
#include <sstream>
#include <QGraphicsSimpleTextItem>
#include "pairwiseIntersections.h"
#include "intersectionOutline.h"
namespace poissonOrdered
{
	intersectionAreaViewer::intersectionAreaViewer(const std::vector<geometry::Circle>& circles, const std::vector<geometry::Line>& lines, const std::vector<geometry::SemiCircle>& semicircles)
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
		//draw outline of intersection region
		std::vector<geometry::Circle> circlesSubset;
		std::vector<geometry::SemiCircle> semicircleSubset;

		if(circles.size() + semicircles.size() > 0)
		{
			QPen noPen;
			noPen.setStyle(Qt::NoPen);
			QBrush redBrush(QColor("red"));
			redBrush.setStyle(Qt::SolidPattern);
			geometry::zipIterator<geometry::Circle, geometry::SemiCircle> pairs(circles, semicircles, 2);
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
	void intersectionAreaViewer::drawSubsetIntersectionOutline(std::vector<geometry::intersectionDescription>& outline)
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
	void intersectionAreaViewer::addOutline(geometry::intersectionDescription& current, geometry::intersectionDescription& next, QPen& redPen, QBrush& noBrush)
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
	intersectionAreaViewer::~intersectionAreaViewer()
	{
		delete graphicsView;
		delete graphicsScene;
	}
	bool intersectionAreaViewer::eventFilter(QObject* object, QEvent *event)
	{
		if(object == graphicsView && event->type() == QEvent::Resize)
		{
			graphicsView->fitInView(minX - abs(minX)*0.01, minY - abs(minX)*0.01, (maxX + abs(maxX) * 0.01) - (minX - abs(minX) * 0.01), (maxY + abs(maxY) * 0.01) - (minY - abs(minY)*0.01), Qt::KeepAspectRatio);
			return false;
		}
		return false;
	}
	bool intersectionAreaViewer::event(QEvent* event)
	{
		if(event->type() == QEvent::WindowStateChange)
		{
			QMainWindow::event(event);
			graphicsView->fitInView(minX - abs(minX)*0.01, minY - abs(minX)*0.01, (maxX + abs(maxX) * 0.01) - (minX - abs(minX) * 0.01), (maxY + abs(maxY) * 0.01) - (minY - abs(minY)*0.01), Qt::KeepAspectRatio);
			return true;
		}
		return false;
	}
}