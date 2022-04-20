#pragma once

/**
 * 面板仪表盘控件
 * 1:可设置范围值,支持负数值
 * 2:可设置精确度+刻度尺精确度,最大支持小数点后3位
 * 3:可设置大刻度数量/小刻度数量
 * 4:可设置开始旋转角度/结束旋转角度
 * 5:可设置是否启用动画效果以及动画效果每次移动的步长
 * 6:可设置刻度颜色+文字颜色+圆环的宽度和颜色
 * 7:自适应窗体拉伸,刻度尺和文字自动缩放
 * 8:可设置单位以及仪表盘名称
 */

#include <QWidget>
#include <QColor>
#include <QString>
#include <QMap>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT GaugePanel : public QWidget
#else
class GaugePanel : public QWidget
#endif
{
    Q_OBJECT
	Q_PROPERTY(double minValue_ READ getMinValue WRITE setMinValue)
	Q_PROPERTY(double maxValue_ READ getMaxValue WRITE setMaxValue)
	Q_PROPERTY(double value_ READ getValue WRITE setValue)
	Q_PROPERTY(int precision_ READ getPrecision WRITE setPrecision)
	Q_PROPERTY(int scalePrecision_ READ getScalePrecision WRITE setScalePrecision)

	Q_PROPERTY(int scaleMajor_ READ getScaleMajor WRITE setScaleMajor)
	Q_PROPERTY(int scaleMinor_ READ getScaleMinor WRITE setScaleMinor)
	Q_PROPERTY(int startAngle_ READ getStartAngle WRITE setStartAngle)
	Q_PROPERTY(int endAngle_ READ getEndAngle WRITE setEndAngle)

	Q_PROPERTY(int ringWidth_ READ getRingWidth WRITE setRingWidth)
	Q_PROPERTY(QMap<double, QColor> ringColorStops_ READ getRingColor WRITE setRingColor)

	Q_PROPERTY(QColor scaleColor_ READ getScaleColor WRITE setScaleColor)
	Q_PROPERTY(QColor pointerColor_ READ getPointerColor WRITE setPointerColor)
	Q_PROPERTY(QColor bgColor_ READ getBgColor WRITE setBgColor)
	Q_PROPERTY(QColor textColor_ READ getTextColor WRITE setTextColor)
	Q_PROPERTY(QString unit_ READ getUnit WRITE setUnit)
	Q_PROPERTY(QString text_ READ getText WRITE setText)

public:
    explicit GaugePanel(QWidget* parent = nullptr);
    ~GaugePanel();

	double getMinValue()     const;
	double getMaxValue()     const;
	double getValue()        const;
	int getPrecision()       const;
	int getScalePrecision()  const;

	int getScaleMajor()      const;
	int getScaleMinor()      const;
	int getStartAngle()      const;
	int getEndAngle()        const;

	int getRingWidth()       const;
	const QMap<double, QColor>& getRingColor() const;

	const QColor& getScaleColor()   const;
	const QColor& getPointerColor() const;
	const QColor& getBgColor()      const;
	const QColor& getTextColor()    const;
	const QString& getUnit()        const;
	const QString& getText()        const;
							 
	QSize sizeHint()         const;
	QSize minimumSizeHint()  const;

public slots:
	//设置范围值
	void setRange(double minValue, double maxValue);

	//设置最大最小值
	void setMinValue(double minValue);
	void setMaxValue(double maxValue);

	//设置目标值
	void setValue(double value);

	//设置精确度
	void setPrecision(int precision);
	//设置刻度尺精确度
	void setScalePrecision(int scalePrecision);

	//设置主刻度数量
	void setScaleMajor(int scaleMajor);
	//设置小刻度数量
	void setScaleMinor(int scaleMinor);
	//设置开始旋转角度
	void setStartAngle(int startAngle);
	//设置结束旋转角度
	void setEndAngle(int endAngle);

	//设置圆环宽度+颜色
	void setRingWidth(int ringWidth);
	void setRingColor(const QMap<double, QColor>& ringColorStops);

	//设置刻度颜色
	void setScaleColor(const QColor& scaleColor);
	//设置指针颜色
	void setPointerColor(const QColor& pointerColor);
	//设置背景颜色
	void setBgColor(const QColor& bgColor);
	//设置文本颜色
	void setTextColor(const QColor& textColor);
	//设置单位
	void setUnit(const QString& unit);
	//设置中间描述文字
	void setText(const QString& text);

protected:
    void paintEvent(QPaintEvent*);
    void drawRing(QPainter* painter);
    void drawScale(QPainter* painter);
    void drawScaleNum(QPainter* painter);
    void drawPointer(QPainter* painter);
    void drawValue(QPainter* painter);

private:
	double minValue_;    //最小值
	double maxValue_;    //最大值
	double value_;       //当前值
	int precision_;      //精确度,小数点后几位
	int scalePrecision_; //刻度尺精确度,小数点后几位

	int scaleMajor_;     //大刻度数量
	int scaleMinor_;     //小刻度数量
	int startAngle_;     //开始旋转角度
	int endAngle_;       //结束旋转角度

	int ringWidth_;      //圆环宽度
	QMap<double, QColor> ringColorStops_;//圆环颜色

	QColor scaleColor_;  //刻度颜色
	QColor pointerColor_;//指针颜色
	QColor bgColor_;     //背景颜色
	QColor textColor_;   //文字颜色
	QString unit_;       //单位
	QString text_;       //描述文字
};
