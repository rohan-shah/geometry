#ifndef INTERSECTION_VIEWER_HEADER_GUARD
#define INTERSECTION_VIEWER_HEADER_GUARD
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include "Shapes.h"
namespace poissonOrdered
{
	class intersectionViewer : public QMainWindow
	{
		Q_OBJECT
	public:
		intersectionViewer(const std::vector<geometry::Circle>& circles);
		~intersectionViewer();
		bool eventFilter(QObject* object, QEvent *event);
	protected:
		virtual bool event(QEvent* event);
	private:
		QGraphicsScene* graphicsScene;
		QGraphicsView* graphicsView;
		std::vector<geometry::Circle> circles;
		float minX, maxX, minY, maxY;
	};
}
#endif