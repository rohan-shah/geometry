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
		void addOutline(const geometry::intersectionDescription& current, const geometry::intersectionDescription& next, QPen& redPen, QBrush& noBrush);
		QGraphicsScene* graphicsScene;
		QGraphicsView* graphicsView;
		std::vector<geometry::Circle> circles;
		std::vector<geometry::Line> lines;
		std::vector<geometry::SemiCircle> semicircles;
		float minX, maxX, minY, maxY;
	};
}
#endif