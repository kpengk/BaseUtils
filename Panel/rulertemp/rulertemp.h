#pragma once

/**
 * 柱状温度计控件
 * 1:可设置精确度(小数点后几位)和间距
 * 2:可设置背景色/柱状颜色/线条颜色
 * 3:可设置长线条步长及短线条步长
 * 4:可启用动画及动画步长
 * 5:可设置范围值
 * 6:支持负数刻度值
 * 7:支持任意窗体大小缩放
 * 8:可设置柱状条位置 左侧 居中 右侧
 * 9:可设置刻度尺位置 无 左侧 右侧 两侧
 * 10:可设置用户设定目标值
 */

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif

class QDESIGNER_WIDGET_EXPORT RulerTemp : public QWidget
#else
class RulerTemp : public QWidget
#endif
{
    Q_OBJECT

	Q_PROPERTY(double minValue_ READ getMinValue WRITE setMinValue)
	Q_PROPERTY(double maxValue_ READ getMaxValue WRITE setMaxValue)
	Q_PROPERTY(double value READ getValue WRITE setValue)

	Q_PROPERTY(int precision_ READ getPrecision WRITE setPrecision)
	Q_PROPERTY(int longStep_ READ getLongStep WRITE setLongStep)
	Q_PROPERTY(int shortStep_ READ getShortStep WRITE setShortStep)
	Q_PROPERTY(int space_ READ getSpace WRITE setSpace)

	Q_PROPERTY(bool showUserValue_ READ getShowUserValue WRITE setShowUserValue)
	Q_PROPERTY(double userValue_ READ getUserValue WRITE setUserValue)
	Q_PROPERTY(QColor userValueColor_ READ getUserValueColor WRITE setUserValueColor)

	Q_PROPERTY(QColor bgColorStart_ READ getBgColorStart WRITE setBgColorStart)
	Q_PROPERTY(QColor bgColorEnd_ READ getBgColorEnd WRITE setBgColorEnd)
	Q_PROPERTY(QColor lineColor_ READ getLineColor WRITE setLineColor)
	Q_PROPERTY(QColor barBgColor_ READ getBarBgColor WRITE setBarBgColor)
	Q_PROPERTY(QColor barColor_ READ getBarColor WRITE setBarColor)

	Q_PROPERTY(BarPosition barPosition_ READ getBarPosition WRITE setBarPosition)
	Q_PROPERTY(TickPosition tickPosition_ READ getTickPosition WRITE setTickPosition)

public:
    enum BarPosition {
        BarPosition_Left = 0,       //左侧显示
        BarPosition_Right = 1,      //右侧显示
        BarPosition_Center = 2      //居中显示
    };
    Q_ENUM(BarPosition)

    enum TickPosition {
        TickPosition_Null = 0,      //不显示
        TickPosition_Left = 1,      //左侧显示
        TickPosition_Right = 2,     //右侧显示
        TickPosition_Both = 3       //两侧显示
    };
    Q_ENUM(TickPosition)

    explicit RulerTemp(QWidget* parent = nullptr);
    ~RulerTemp();

    double getMinValue()            const;
    double getMaxValue()            const;
    double getValue()               const;

    int getPrecision()              const;
    int getLongStep()               const;
    int getShortStep()              const;
    int getSpace()                  const;

    bool getShowUserValue()         const;
    double getUserValue()           const;
    QColor getUserValueColor()      const;

    QColor getBgColorStart()        const;
    QColor getBgColorEnd()          const;
    QColor getLineColor()           const;
    QColor getBarBgColor()          const;
    QColor getBarColor()            const;

    BarPosition getBarPosition()    const;
    TickPosition getTickPosition()  const;

    QSize sizeHint()                const;
    QSize minimumSizeHint()         const;

public slots:
    //设置最大最小值-范围值
    void setRange(double minValue, double maxValue);
    void setRange(int minValue, int maxValue);

    //设置最大最小值
    void setMinValue(double minValue);
    void setMaxValue(double maxValue);

    //设置目标值
    void setValue(double value);
    void setValue(int value);

    //设置精确度
    void setPrecision(int precision);

    //设置线条等分步长
    void setLongStep(int longStep);
    void setShortStep(int shortStep);

    //设置间距
    void setSpace(int space);

    //设置是否显示用户设定值
    void setShowUserValue(bool showUserValue);

    //设置用户值
    void setUserValue(double userValue);
    void setUserValue(int userValue);

    //设置用户设定值颜色
    void setUserValueColor(const QColor& userValueColor);

    //设置背景颜色
    void setBgColorStart(const QColor& bgColorStart);
    void setBgColorEnd(const QColor& bgColorEnd);

    //设置线条颜色
    void setLineColor(const QColor& lineColor);

    //设置柱状颜色
    void setBarBgColor(const QColor& barBgColor);
    void setBarColor(const QColor& barColor);

    //设置柱状条位置
    void setBarPosition(const BarPosition& barPosition);

    //设置刻度尺位置
    void setTickPosition(const TickPosition& tickPosition);

protected:
	void resizeEvent(QResizeEvent*);
	void paintEvent(QPaintEvent*);
	void drawBg(QPainter* painter);
	void drawBarBg(QPainter* painter);
	void drawRuler(QPainter* painter, int type);
	void drawBar(QPainter* painter);
	void drawValue(QPainter* painter);

private:
	double minValue_;          //最小值
	double maxValue_;          //最大值
	double value_;             //当前值

	int precision_;            //精确度,小数点后几位
	int longStep_;             //长线条等分步长
	int shortStep_;            //短线条等分步长
	int space_;                //间距

	bool showUserValue_;       //显示用户设定值
	double userValue_;         //用户设定值
	QColor userValueColor_;    //用户设定值颜色

	QColor bgColorStart_;      //背景渐变开始颜色
	QColor bgColorEnd_;        //背景渐变结束颜色
	QColor lineColor_;         //线条颜色
	QColor barBgColor_;        //柱状背景色
	QColor barColor_;          //柱状颜色

	BarPosition barPosition_;  //柱状条位置
	TickPosition tickPosition_;//刻度尺位置

	int barWidth_;             //水银柱宽度
	int barHeight_;            //水银柱高度
	int radius_;               //水银柱底部圆半径
	int targetX_;              //目标X坐标
	QRectF barRect_;           //柱状区域
	QRectF circleRect_;        //底部圆区域
};
