#ifndef INTERSECTION_AREA_VIEWER_HEADER_GUARD
#define INTERSECTION_AREA_VIEWER_HEADER_GUARD
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include "Shapes.h"
namespace poissonOrdered
{
	class intersectionAreaViewer : public QMainWindow
	{
		Q_OBJECT
	public:
		intersectionAreaViewer(const std::vector<geometry::Circle>& circles, const std::vector<geometry::Line>& lines, const std::vector<geometry::SemiCircle>& semicircles);
		~intersectionAreaViewer();
		bool eventFilter(QObject* object, QEvent *event);
	protected:
		virtual bool event(QEvent* event);
	private:
		//Note that the final argument is required to handle the case where we have only two circles. 
		void addOutline(geometry::intersectionDescription& current, geometry::intersectionDescription& next, QPen& redPen, QBrush& noBrush);
		void drawSubsetIntersectionOutline(std::vector<geometry::intersectionDescription>& allIntersections);
		QGraphicsScene* graphicsScene;
		QGraphicsView* graphicsView;
		std::vector<geometry::Circle> circles;
		std::vector<geometry::Line> lines;
		std::vector<geometry::SemiCircle> semicircles;
		float minX, maxX, minY, maxY;
	};
}
#endif