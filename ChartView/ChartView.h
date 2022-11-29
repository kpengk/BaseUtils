#pragma once
#include <QChartView>
#include <QMouseEvent>
#include <QGraphicsSimpleTextItem>

#if (QT_VERSION <= QT_VERSION_CHECK(6,0,0))
QT_CHARTS_USE_NAMESPACE
#endif

class ChartView : public QChartView
{
	Q_OBJECT

    using AxisRange = std::array<qreal, 2>;

public:
	explicit ChartView(QWidget* parent = nullptr);
    ChartView(QChart *chart, QWidget *parent = nullptr);
	~ChartView();
	// Save the coordinate area for resetting
	void saveAxisRange();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);

private:
	QPoint prevPoint_;
	bool leftButtonPressed_;
	bool ctrlPressed_;
	bool alreadySaveRange_;
    AxisRange xRange_;
    AxisRange yRange_;
	QGraphicsSimpleTextItem* coordItem_;
};
