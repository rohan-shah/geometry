#include "convexBoundaryViewer.h"
#include <limits>
#include <QEvent>
#include "pairwiseIntersections.h"
namespace poissonOrdered
{
	convexBoundaryViewer::convexBoundaryViewer(const std::vector<geometry::Circle>& circles, const std::vector<geometry::Line>& lines, const std::vector<geometry::SemiCircle>& semicircles)
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
		if(circles.size() == 0)
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
		QPen pen(QColor("black"));
		pen.setStyle(Qt::SolidLine);
		pen.setWidthF(0.01);

		this->resize(this->width(), this->height());

		for(std::vector<geometry::Circle>::const_iterator i = circles.begin(); i!=circles.end();i++)
		{
			graphicsScene->addEllipse(i->pos[0] - i->radius, i->pos[1] - i->radius, 2*i->radius, 2*i->radius, pen);
		}
		for(std::vector<geometry::SemiCircle>::const_iterator i = semicircles.begin(); i != semicircles.end();i++)
		{
			cml::vector2f point = i->circle.pos;
			cml::vector2f line = i->straightLine.unitDirection;
			line /= line.length_squared();
			float radius = i->circle.radius;
			QPainterPath path;
			path.arcMoveTo(point[0]-radius, point[1]-radius, 2*radius, 2*radius, atan2(line[1], line[0])*360/(2*M_PI));
			path.arcTo(point[0]-radius, point[1]-radius, 2*radius, 2*radius, atan2(line[1], line[0])*360/(2*M_PI), 180);
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
		std::vector<geometry::intersectionDescription> result;
		geometry::pairwiseIntersections(circles, lines, semicircles, result);
		QPen noPen;
		noPen.setStyle(Qt::NoPen);
		QBrush redBrush(QColor("red"));
		redBrush.setStyle(Qt::SolidPattern);
		for(std::vector<geometry::intersectionDescription>::const_iterator i = result.begin(); i!= result.end(); i++)
		{
			graphicsScene->addEllipse(i->point[0]- 0.025, i->point[1] - 0.025, 0.05, 0.05, noPen, redBrush);
		}
	}
	convexBoundaryViewer::~convexBoundaryViewer()
	{
		delete graphicsView;
		delete graphicsScene;
	}
	bool convexBoundaryViewer::eventFilter(QObject* object, QEvent *event)
	{
		if(object == graphicsView && event->type() == QEvent::Resize)
		{
			graphicsView->fitInView(minX - abs(minX)*0.01, minY - abs(minX)*0.01, (maxX + abs(maxX) * 0.01) - (minX - abs(minX) * 0.01), (maxY + abs(maxY) * 0.01) - (minY - abs(minY)*0.01), Qt::KeepAspectRatio);
			return false;
		}
		return false;
	}
	bool convexBoundaryViewer::event(QEvent* event)
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