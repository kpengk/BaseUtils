#ifndef GAUGEPANEL_H
#define GAUGEPANEL_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QPainter>

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
	Q_PROPERTY(double value_ READ getValue WRITE setValue)
	Q_PROPERTY(double lower_ READ getLower WRITE setLower)
	Q_PROPERTY(double upper_ READ getUpper WRITE setUpper)
	Q_PROPERTY(int hShearValue_ READ getHShearValue WRITE setHShearValue)
	Q_PROPERTY(int vShearValue_ READ getVShearValue WRITE setVShearValue)
	Q_PROPERTY(double radiusInner_ READ getRadiusInner WRITE setRadiusInner)
	Q_PROPERTY(double radiusOuter_ READ getRadiusOuter WRITE setRadiusOuter)
	Q_PROPERTY(double radiusHalo_ READ getRadiusHalo WRITE setRadiusHalo)
	Q_PROPERTY(QColor colorOuterFrame_ READ getColorOuterFrame WRITE setColorOuterFrame)
	Q_PROPERTY(QColor colorInnerStart_ READ getColorInnerStart WRITE setColorInnerStart)
	Q_PROPERTY(QColor colorInnerEnd_ READ getColorInnerEnd WRITE setColorInnerEnd)
	Q_PROPERTY(QColor colorOuterStart_ READ getColorOuterStart WRITE setColorOuterStart)
	Q_PROPERTY(QColor colorOuterEnd_ READ getColorOuterEnd WRITE setColorOuterEnd)
	Q_PROPERTY(QColor colorHaloStart_ READ getColorHaloStart WRITE setColorHaloStart)
	Q_PROPERTY(QColor colorHaloEnd_ READ getColorHaloEnd WRITE setColorHaloEnd)

Q_SIGNALS:
	void valueChanged(qreal value);

public:
	explicit GaugePanel(QWidget* parent = nullptr);
	~GaugePanel();

	double getValue()           const;
	double getLower()           const;
	double getUpper()           const;
	int    getHShearValue()     const;
	int    getVShearValue()     const;
	double getRadiusInner()     const;
	double getRadiusOuter()     const;
	double getRadiusHalo()      const;
	QColor getColorOuterFrame() const;
	QColor getColorInnerStart() const;
	QColor getColorInnerEnd()   const;
	QColor getColorOuterStart() const;
	QColor getColorOuterEnd()   const;
	QColor getColorHaloStart()  const;
	QColor getColorHaloEnd()    const;

	void setValue(int value);
	void setValue(double value);
	void setLower(double value);
	void setUpper(double value);
	void setValueRange(double lower, double upper);
	void setHShearValue(int value);
	void setVShearValue(int value);

	//表盘外边框颜色
	void setColorOuterFrame(QColor color);

	//内层渐变区半径
	void setRadiusInner(int radius);
	void setRadiusInner(double radius);

	//外层渐变区半径
	void setRadiusOuter(int radius);
	void setRadiusOuter(double radius);

	//外层光晕区半径
	void setRadiusHalo(int radius);
	void setRadiusHalo(double radius);

	//内层渐变颜色
	void setColorInnerStart(QColor color);
	void setColorInnerEnd(QColor color);

	//外层渐变颜色
	void setColorOuterStart(QColor color);
	void setColorOuterEnd(QColor color);

	//光晕颜色
	void setColorHaloStart(QColor color);
	void setColorHaloEnd(QColor color);

	void startShearAnimal(int duration, int hShearValue, int vShearValue);

public slots:
	void updateValue(double value);

protected:
	void paintEvent(QPaintEvent*);

private:
	void drawOuterGradient(QPainter* painter);
	void drawInnerGradient(QPainter* painter);
	void drawOuterHalo(QPainter* painter);
	void drawScale(QPainter* painter);
	void drawScaleNum(QPainter* painter);
	void drawPointer(QPainter* painter);
	void drawPointerSector(QPainter* painter);
	void drawValue(QPainter* painter);
	void drawUnit(QPainter* painter);

private:
	double value_;          //目标值
	double lower_;			//最低值
	double upper_;			//最高值
	int hShearValue_;       //H、V扭曲值
	int vShearValue_;       //H、V扭曲值
	double radiusInner_;    //渐变内圈内半径
	double radiusOuter_;    //渐变外圈内半径
	double radiusHalo_;     //光晕内半径
	QColor colorOuterFrame_;//表盘外边框颜色
	QColor colorInnerStart_;//渐变内圈起始颜色
	QColor colorInnerEnd_;  //渐变内圈结束颜色
	QColor colorOuterStart_;//渐变外圈起始颜色
	QColor colorOuterEnd_;  //渐变外圈结束颜色
	QColor colorHaloStart_; //光晕起始颜色
	QColor colorHaloEnd_;   //光晕结束颜色

	QPropertyAnimation* hShearAnimation;
	QPropertyAnimation* vShearAnimation;
};

#endif // GaugePanel_H

