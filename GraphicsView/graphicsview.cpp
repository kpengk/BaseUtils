#include "graphicsview.hpp"

#include <QtMath>
#include <QtWidgets>
//#include <QtOpenGL>
#include <QBoxLayout>

GraphicsView::GraphicsView(QWidget* parent)
	: QGraphicsView{ parent }
	, zoomValue_{ 250 }//0~500
	, rotateX_{}
	, rotateY_{}
	, rotateZ_{}
{
	setRenderHint(QPainter::Antialiasing, true);
	setDragMode(QGraphicsView::ScrollHandDrag);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	//setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

	setupMatrix();
}

GraphicsView::GraphicsView(QGraphicsScene* scene, QWidget* parent)
	: GraphicsView{ parent }
{
	setScene(scene);
}

GraphicsView::~GraphicsView()
{
}

void GraphicsView::wheelEvent(QWheelEvent* event)
{
	if (event->angleDelta().y() > 0)
		zoomIn(6);
	else
		zoomOut(6);
	event->accept();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton) {
		resetView();
		event->accept();
	}
	else {
		QGraphicsView::mouseReleaseEvent(event);
	}
}

void GraphicsView::resetView()
{
	zoomValue_ = 250;
	rotateX_ = 0;
	rotateY_ = 0;
	rotateZ_ = 0;
	setupMatrix();
	ensureVisible(QRectF(0, 0, 0, 0));

	emit reset();
}

void GraphicsView::setupMatrix()
{
	const qreal scale = qPow(qreal(2), (zoomValue_ - 250) / qreal(50));

	QTransform matrix;
	matrix.scale(scale, scale);
	matrix.rotate(rotateX_, Qt::XAxis);
	matrix.rotate(rotateY_, Qt::YAxis);
	matrix.rotate(rotateZ_, Qt::ZAxis);
	setTransform(matrix);
}

void GraphicsView::zoomIn(int level)
{
	if (zoomValue_ >= 500)
		return;

	zoomValue_ += level;
	setupMatrix();
}

void GraphicsView::zoomOut(int level)
{
	if (zoomValue_ <= 0)
		return;

	zoomValue_ -= level;
	setupMatrix();
}

void GraphicsView::rotateX(qreal angle)
{
	rotateX_ = angle;
	setupMatrix();
}

void GraphicsView::rotateY(qreal angle)
{
	rotateY_ = angle;
	setupMatrix();
}

void GraphicsView::rotateZ(qreal angle)
{
	rotateZ_ = angle;
	setupMatrix();
}
