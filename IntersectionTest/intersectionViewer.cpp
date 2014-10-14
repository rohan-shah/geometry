#include "intersectionViewer.h"
#include <limits>
#include <QEvent>
#include "pairwiseIntersections.h"
namespace poissonOrdered
{
	intersectionViewer::intersectionViewer(const std::vector<geometry::Circle>& circles)
		:circles(circles)
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
		graphicsScene->addLine(minX, minY, maxX, minY, pen);
		graphicsScene->addLine(maxX, minY, maxX, maxY, pen);
		graphicsScene->addLine(minX, maxY, maxX, maxY, pen);
		graphicsScene->addLine(minX, minY, minX, maxY, pen);

		for(std::vector<geometry::Circle>::const_iterator i = circles.begin(); i!=circles.end();i++)
		{
			graphicsScene->addEllipse(i->pos[0] - i->radius, i->pos[1] - i->radius, 2*i->radius, 2*i->radius, pen);
		}

		std::vector<geometry::intersectionDescription> result;
		std::vector<geometry::Line> lines;
		std::vector<geometry::SemiCircle> semicircles;
		pairwiseIntersections(circles, lines, semicircles, result);
		QPen noPen;
		noPen.setStyle(Qt::NoPen);
		QBrush redBrush(QColor("red"));
		redBrush.setStyle(Qt::SolidPattern);
		for(std::vector<geometry::intersectionDescription>::const_iterator i = result.begin(); i!= result.end(); i++)
		{
			graphicsScene->addEllipse(i->point[0]- 0.025, i->point[1] - 0.025, 0.05, 0.05, noPen, redBrush);
		}
	}
	intersectionViewer::~intersectionViewer()
	{
		delete graphicsView;
		delete graphicsScene;
	}
	bool intersectionViewer::eventFilter(QObject* object, QEvent *event)
	{
		if(object == graphicsView && event->type() == QEvent::Resize)
		{
			graphicsView->fitInView(minX - abs(minX)*0.01, minY - abs(minX)*0.01, (maxX + abs(maxX) * 0.01) - (minX - abs(minX) * 0.01), (maxY + abs(maxY) * 0.01) - (minY - abs(minY)*0.01), Qt::KeepAspectRatio);
			return false;
		}
		return false;
	}
	bool intersectionViewer::event(QEvent* event)
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