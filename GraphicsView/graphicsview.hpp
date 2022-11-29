#pragma once

#include <QWidget>
#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
	Q_OBJECT

signals:
	void reset();
	void key_minus();
	void key_plus();

public:
	explicit GraphicsView(QWidget* parent = nullptr);
	explicit GraphicsView(QGraphicsScene* scene, QWidget* parent = nullptr);
	~GraphicsView();

protected:
	void wheelEvent(QWheelEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

public slots:
	void zoom_in(int level = 1);
	void zoom_out(int level = 1);
	void rotate_x(qreal angle);
	void rotate_y(qreal angle);
	void rotate_z(qreal angle);
	void reset_view();
	void setup_matrix();

private:
	int zoom_value_;
	double rotate_x_;
	double rotate_y_;
	double rotate_z_;
};
