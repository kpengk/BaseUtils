#pragma once

/**
 * 汽车仪表盘控件
 * 1:可设置范围值,支持负数值
 * 2:可设置精确度,最大支持小数点后3位
 * 3:可设置大刻度数量/小刻度数量
 * 4:可设置开始旋转角度/结束旋转角度
 * 5:可设置是否启用动画效果以及动画效果每次移动的步长
 * 6:可设置外圆背景/内圆背景/饼圆三种颜色/刻度尺颜色/文字颜色
 * 7:自适应窗体拉伸,刻度尺和文字自动缩放
 * 8:可自由拓展各种渐变色,各圆的半径
 * 9:三色圆环按照比例设置范围角度 用户可以自由设置三色占用比例
 * 10:圆环样式可选择 三色圆环 当前圆环
 * 11:指示器样式可选择 圆形指示器 指针指示器 圆角指针指示器 三角形指示器
 */

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT GaugeCar : public QWidget
#else
class GaugeCar : public QWidget
#endif
{
    Q_OBJECT
	Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
	Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
	Q_PROPERTY(double value READ value WRITE setValue)
	Q_PROPERTY(int precision READ precision WRITE setPrecision)
	Q_PROPERTY(int scaleMajor READ scaleMajor WRITE setScaleMajor)
	Q_PROPERTY(int scaleMinor READ scaleMinor WRITE setScaleMinor)
	Q_PROPERTY(int startAngle READ startAngle WRITE setStartAngle)
	Q_PROPERTY(int endAngle READ endAngle WRITE setEndAngle)
	Q_PROPERTY(QColor outerCircleColor READ outerCircleColor WRITE setOuterCircleColor)
	Q_PROPERTY(QColor innerCircleColor READ innerCircleColor WRITE setInnerCircleColor)
	Q_PROPERTY(QColor pieColorStart READ pieColorStart WRITE setPieColorStart)
	Q_PROPERTY(QColor pieColorMid READ pieColorMid WRITE setPieColorMid)
	Q_PROPERTY(QColor pieColorEnd READ pieColorEnd WRITE setPieColorEnd)
	Q_PROPERTY(QColor coverCircleColor READ coverCircleColor WRITE setCoverCircleColor)
	Q_PROPERTY(QColor scaleColor READ scaleColor WRITE setScaleColor)
	Q_PROPERTY(QColor pointerColor READ pointerColor WRITE setPointerColor)
	Q_PROPERTY(QColor centerCircleColor READ centerCircleColor WRITE setCenterCircleColor)
	Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)
	Q_PROPERTY(bool showOverlay READ showOverlay WRITE setShowOverlay)
	Q_PROPERTY(QColor overlayColor READ overlayColor WRITE setOverlayColor)
	Q_PROPERTY(PieStyle pieStyle READ pieStyle WRITE setPieStyle)
	Q_PROPERTY(PointerStyle pointerStyle READ pointerStyle WRITE setPointerStyle)

public:
	enum PieStyle {
		PieStyle_Three = 0,         	//三色圆环
		PieStyle_Current = 1        	//当前圆环
	};
	Q_ENUM(PieStyle)

	enum PointerStyle {
		PointerStyle_Circle = 0,        //圆形指示器
		PointerStyle_Indicator = 1,     //指针指示器
		PointerStyle_IndicatorR = 2,    //圆角指针指示器
		PointerStyle_Triangle = 3       //三角形指示器
	};
	Q_ENUM(PointerStyle)

public:
    explicit GaugeCar(QWidget* parent = nullptr);
    ~GaugeCar();

	QSize sizeHint()                const;
	QSize minimumSizeHint()         const;

    double minValue() const;
    double maxValue() const;
    double value() const;
    int precision() const;
    int scaleMajor() const;
    int scaleMinor() const;
    int startAngle() const;
    int endAngle() const;
	const QColor& outerCircleColor() const;
	const QColor& innerCircleColor() const;
	const QColor& pieColorStart() const;
	const QColor& pieColorMid() const;
	const QColor& pieColorEnd() const;
	const QColor& coverCircleColor() const;
	const QColor& scaleColor() const;
	const QColor& pointerColor() const;
	const QColor& centerCircleColor() const;
	const QColor& textColor() const;
	const QColor& overlayColor() const;
	PieStyle pieStyle() const;
	PointerStyle pointerStyle() const;

public slots:
	//设置范围值
	void setRange(double minValue, double maxValue);
	void setMinValue(double newMinValue);
	void setMaxValue(double newMaxValue);
	void setValue(double newValue);
	void setPrecision(int newPrecision);
	void setScaleMajor(int newScaleMajor);
	void setScaleMinor(int newScaleMinor);
	void setStartAngle(int newStartAngle);
	void setEndAngle(int newEndAngle);
	void setOuterCircleColor(const QColor& newOuterCircleColor);
	void setInnerCircleColor(const QColor& newInnerCircleColor);
	void setPieColorStart(const QColor& newPieColorStart);
	void setPieColorMid(const QColor& newPieColorMid);
	void setPieColorEnd(const QColor& newPieColorEnd);
	void setCoverCircleColor(const QColor& newCoverCircleColor);
	void setScaleColor(const QColor& newScaleColor);
	void setPointerColor(const QColor& newPointerColor);
	void setCenterCircleColor(const QColor& newCenterCircleColor);
	void setTextColor(const QColor& newTextColor);
	bool showOverlay() const;
	void setShowOverlay(bool newShowOverlay);
	void setOverlayColor(const QColor& newOverlayColor);
	void setPieStyle(PieStyle newPieStyle);
	void setPointerStyle(PointerStyle newPointerStyle);

protected:
    void paintEvent(QPaintEvent*);
    void drawOuterCircle(QPainter* painter);
    void drawInnerCircle(QPainter* painter);
    void drawColorPie(QPainter* painter);
    void drawCoverCircle(QPainter* painter);
    void drawScale(QPainter* painter);
    void drawScaleNum(QPainter* painter);
    void drawPointerCircle(QPainter* painter);
    void drawPointerIndicator(QPainter* painter);
    void drawPointerIndicatorR(QPainter* painter);
    void drawPointerTriangle(QPainter* painter);
    void drawRoundCircle(QPainter* painter);
    void drawCenterCircle(QPainter* painter);
    void drawValue(QPainter* painter);
    void drawOverlay(QPainter* painter);

private:
	double minValue_;//最小值
	double maxValue_;//最大值
	double value_;   //当前值
	int precision_;  //精确度,小数点后几位

	int scaleMajor_; //大刻度数量
	int scaleMinor_; //小刻度数量
	int startAngle_; //开始旋转角度
	int endAngle_;   //结束旋转角度

	QColor outerCircleColor_; //外圆背景颜色
	QColor innerCircleColor_; //内圆背景颜色

	QColor pieColorStart_;    //饼圆开始颜色
	QColor pieColorMid_;      //饼圆中间颜色
	QColor pieColorEnd_;      //饼圆结束颜色

	QColor coverCircleColor_; //覆盖圆背景颜色
	QColor scaleColor_;       //刻度尺颜色
	QColor pointerColor_;     //指针颜色
	QColor centerCircleColor_;//中心圆颜色
	QColor textColor_;        //文字颜色

	bool showOverlay_;        //显示遮罩层
	QColor overlayColor_;     //遮罩层颜色

	PieStyle pieStyle_;        //饼图样式
	PointerStyle pointerStyle_;//指针样式
};
