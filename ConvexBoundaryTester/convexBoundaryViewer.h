#ifndef CONVEX_BOUNDARY_VIEWER_HEADER_GUARD
#define CONVEX_BOUNDARY_VIEWER_HEADER_GUARD
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include "Shapes.h"
namespace poissonOrdered
{
	class convexBoundaryViewer : public QMainWindow
	{
		Q_OBJECT
	public:
		convexBoundaryViewer(const std::vector<geometry::Circle>& circles, const std::vector<geometry::Line>& lines, const std::vector<geometry::SemiCircle>& semicircles);
		~convexBoundaryViewer();
		bool eventFilter(QObject* object, QEvent *event);
	protected:
		virtual bool event(QEvent* event);
	private:
		QGraphicsScene* graphicsScene;
		QGraphicsView* graphicsView;
		std::vector<geometry::Circle> circles;
		std::vector<geometry::Line> lines;
		std::vector<geometry::SemiCircle> semicircles;
		float minX, maxX, minY, maxY;
	};
}
#endif