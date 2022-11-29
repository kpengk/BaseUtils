#include "graphicsview.hpp"

#include <QtMath>
#include <QtWidgets>
//#include <QtOpenGL>
#include <QBoxLayout>

GraphicsView::GraphicsView(QWidget* parent)
	: QGraphicsView{ parent },
	zoom_value_{ 250 },  // 0~500
	rotate_x_{},
	rotate_y_{},
	rotate_z_{}
{
	setRenderHint(QPainter::Antialiasing, true);
	setDragMode(QGraphicsView::ScrollHandDrag);
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	//setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

	setup_matrix();
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
		zoom_in(6);
	else
		zoom_out(6);
	event->accept();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::RightButton) {
		reset_view();
		event->accept();
		return;
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::keyPressEvent(QKeyEvent* event)
{
	if (event->modifiers() == Qt::CTRL)
	{
		if (event->key() == Qt::Key_Minus)
		{
			emit key_minus();
			event->accept();
			return;
		}
		else if (event->key() == Qt::Key_Equal)
		{
			emit key_plus();
			event->accept();
			return;
		}
	}
	QGraphicsView::keyPressEvent(event);
}

void GraphicsView::reset_view()
{
	zoom_value_ = 250;
	rotate_x_ = 0;
	rotate_y_ = 0;
	rotate_z_ = 0;
	setup_matrix();
	ensureVisible(QRectF(0, 0, 0, 0));

	emit reset();
}

void GraphicsView::setup_matrix()
{
	const qreal scale = qPow(qreal(2), (zoom_value_ - 250) / qreal(50));

	QTransform matrix;
	matrix.scale(scale, scale);
	matrix.rotate(rotate_x_, Qt::XAxis);
	matrix.rotate(rotate_y_, Qt::YAxis);
	matrix.rotate(rotate_z_, Qt::ZAxis);
	setTransform(matrix);
}

void GraphicsView::zoom_in(int level)
{
	if (zoom_value_ >= 500)
		return;

	zoom_value_ += level;
	setup_matrix();
}

void GraphicsView::zoom_out(int level)
{
	if (zoom_value_ <= 0)
		return;

	zoom_value_ -= level;
	setup_matrix();
}

void GraphicsView::rotate_x(qreal angle)
{
	rotate_x_ = angle;
	setup_matrix();
}

void GraphicsView::rotate_y(qreal angle)
{
	rotate_y_ = angle;
	setup_matrix();
}

void GraphicsView::rotate_z(qreal angle)
{
	rotate_z_ = angle;
	setup_matrix();
}
