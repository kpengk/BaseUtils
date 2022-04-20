#pragma once

/**
 * 圆弧仪表盘控件
 * 1:可设置范围值,支持负数值
 * 2:可设置精确度,最大支持小数点后3位
 * 3:可设置大刻度数量/小刻度数量
 * 4:可设置开始旋转角度/结束旋转角度
 * 5:可设置是否启用动画效果以及动画效果每次移动的步长
 * 6:可设置外圆背景/内圆背景/饼圆三种颜色/刻度尺颜色/文字颜色
 * 7:自适应窗体拉伸,刻度尺和文字自动缩放
 * 8:可自由拓展各种渐变色,各圆的半径
 * 9:指示器样式可选择 圆形指示器 指针指示器 圆角指针指示器 三角形指示器
 */

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT GaugeArc : public QWidget
#else
class GaugeArc : public QWidget
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
    Q_PROPERTY(QColor arcColor READ arcColor WRITE setArcColor)
    Q_PROPERTY(QColor scaleColor READ scaleColor WRITE setScaleColor)
    Q_PROPERTY(QColor scaleNumColor READ scaleNumColor WRITE setScaleNumColor)
    Q_PROPERTY(QColor pointerColor READ pointerColor WRITE setPointerColor)
    Q_PROPERTY(QColor textColor READ textColor WRITE setTextColor)

public:
    enum PointerStyle {
        PointerStyle_Circle = 0,        //圆形指示器
        PointerStyle_Indicator = 1,     //指针指示器
        PointerStyle_IndicatorR = 2,    //圆角指针指示器
        PointerStyle_Triangle = 3       //三角形指示器
    };
    Q_ENUM(PointerStyle)

    explicit GaugeArc(QWidget* parent = nullptr);
    ~GaugeArc();

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
    const QColor &arcColor() const;
    const QColor &scaleColor() const;
    const QColor &pointerColor() const;
    const QColor &textColor() const;
    const QColor &scaleNumColor() const;

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
    void setArcColor(const QColor &newArcColor);
    void setScaleColor(const QColor &newScaleColor);
    void setPointerColor(const QColor &newPointerColor);
    void setTextColor(const QColor &newTextColor);
    void setScaleNumColor(const QColor &newScaleNumColor);

protected:
    void paintEvent(QPaintEvent*);
    void drawArc(QPainter* painter);
    void drawScale(QPainter* painter);
    void drawScaleNum(QPainter* painter);
    void drawPointerCircle(QPainter* painter);
    void drawPointerIndicator(QPainter* painter);
    void drawPointerIndicatorR(QPainter* painter);
    void drawPointerTriangle(QPainter* painter);
    void drawRoundCircle(QPainter* painter);
    void drawCenterCircle(QPainter* painter);
    void drawValue(QPainter* painter);

private:
    double minValue_;//最小值
    double maxValue_;//最大值
    double value_;   //当前值
    int precision_;  //精确度,小数点后几位

    int scaleMajor_; //大刻度数量
    int scaleMinor_; //小刻度数量
    int startAngle_; //开始旋转角度
    int endAngle_;   //结束旋转角度

    QColor arcColor_;     //圆弧颜色
    QColor scaleColor_;   //刻度尺颜色
    QColor scaleNumColor_;//刻度值颜色
    QColor pointerColor_; //指针颜色
    QColor textColor_;    //文字颜色

    PointerStyle pointerStyle_;//指针样式
};
