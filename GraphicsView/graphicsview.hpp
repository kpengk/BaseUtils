#pragma once

#include <QWidget>
#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

signals:
	void reset();

public:
	explicit GraphicsView(QWidget* parent = nullptr);
	GraphicsView(QGraphicsScene* scene, QWidget* parent = nullptr);
	~GraphicsView();

protected:
	void wheelEvent(QWheelEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

public slots:
	void zoomIn(int level = 1);
	void zoomOut(int level = 1);
	void rotateX(qreal angle);
	void rotateY(qreal angle);
	void rotateZ(qreal angle);
	void resetView();
	void setupMatrix();

private:
	int zoomValue_;
	double rotateX_;
	double rotateY_;
	double rotateZ_;
};
