#include "gaugearc.h"
#include <QPainter>

GaugeArc::GaugeArc(QWidget* parent)
	:QWidget(parent)
{
	minValue_ = 0.0;
	maxValue_ = 100.0;
	value_ = 83.0;
	precision_ = 1;

	scaleMajor_ = 10;
	scaleMinor_ = 10;
	startAngle_ = 45;
	endAngle_ = 45;

	arcColor_ = QColor(15, 244, 251);
	scaleColor_ = QColor(15, 244, 251);
	scaleNumColor_ = QColor(15, 244, 251);
	pointerColor_ = QColor(255, 107, 107);
	textColor_ = QColor(104, 181, 249);

	pointerStyle_ = PointerStyle::PointerStyle_Indicator;
}

GaugeArc::~GaugeArc()
{

}

void GaugeArc::paintEvent(QPaintEvent*)
{
	int width = this->width();
	int height = this->height();
	int side = qMin(width, height);

	//绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.translate(width / 2, height / 2);
	painter.scale(side / 200.0, side / 200.0);

	//绘制圆弧
	drawArc(&painter);
	//绘制刻度线
	drawScale(&painter);
	//绘制刻度值
	drawScaleNum(&painter);

	//根据指示器形状绘制指示器
	if (pointerStyle_ == PointerStyle_Circle) {
		drawPointerCircle(&painter);
	}
	else if (pointerStyle_ == PointerStyle_Indicator) {
		drawPointerIndicator(&painter);
	}
	else if (pointerStyle_ == PointerStyle_IndicatorR) {
		drawPointerIndicatorR(&painter);
	}
	else if (pointerStyle_ == PointerStyle_Triangle) {
		drawPointerTriangle(&painter);
	}

	//绘制指针中心圆外边框
	drawRoundCircle(&painter);
	//绘制指针中心圆
	drawCenterCircle(&painter);
	//绘制当前值
	drawValue(&painter);
}

void GaugeArc::drawArc(QPainter* painter)
{
	int radius = 98;
	painter->save();
	QPen pen;
	pen.setColor(arcColor_);
	pen.setWidthF(2);

	painter->setPen(pen);
	painter->setBrush(Qt::NoBrush);

	//计算总范围角度,当前值范围角度,剩余值范围角度
	double angleAll = 360.0 - startAngle_ - endAngle_;
	double angleCurrent = angleAll * ((value_ - minValue_) / (maxValue_ - minValue_));
	double angleOther = angleAll - angleCurrent;

	//绘制外边框圆弧
	QRectF rect = QRectF(-radius, -radius, radius * 2, radius * 2);
	painter->drawArc(rect, (270 - startAngle_ - angleCurrent) * 16, angleCurrent * 16);
	painter->drawArc(rect, (270 - startAngle_ - angleCurrent - angleOther) * 16, angleOther * 16);

	//绘制里边框圆弧
	radius = 90;
	rect = QRectF(-radius, -radius, radius * 2, radius * 2);
	painter->drawArc(rect, (270 - startAngle_ - angleCurrent) * 16, angleCurrent * 16);
	painter->drawArc(rect, (270 - startAngle_ - angleCurrent - angleOther) * 16, angleOther * 16);

	painter->restore();
}

void GaugeArc::drawScale(QPainter* painter)
{
	int radius = 97;
	painter->save();

	painter->rotate(startAngle_);
	int steps = (scaleMajor_ * scaleMinor_);
	double angleStep = (360.0 - startAngle_ - endAngle_) / steps;

	QPen pen;
	pen.setColor(scaleColor_);
	pen.setCapStyle(Qt::RoundCap);

	for (int i = 0; i <= steps; i++) {
		if (i % scaleMinor_ == 0) {
			pen.setWidthF(1.5);
			painter->setPen(pen);
			painter->drawLine(0, radius - 12, 0, radius);
		}
		else {
			pen.setWidthF(1.0);
			painter->setPen(pen);
			painter->drawLine(0, radius - 5, 0, radius);
		}

		painter->rotate(angleStep);
	}

	painter->restore();
}

void GaugeArc::drawScaleNum(QPainter* painter)
{
	int radius = 75;
	painter->save();
	painter->setPen(scaleNumColor_);

	double startRad = (360 - startAngle_ - 90) * (M_PI / 180);
	double deltaRad = (360 - startAngle_ - endAngle_) * (M_PI / 180) / scaleMajor_;

	for (int i = 0; i <= scaleMajor_; i++) {
		double sina = qSin(startRad - i * deltaRad);
		double cosa = qCos(startRad - i * deltaRad);
		double value = 1.0 * i * ((maxValue_ - minValue_) / scaleMajor_) + minValue_;

		QString strValue = QString("%1").arg((double)value, 0, 'f', precision_);
		const QRect rect = fontMetrics().boundingRect(strValue);
		const double textWidth = rect.width();
		const double textHeight = rect.height();
		int x = radius * cosa - textWidth / 2;
		int y = -radius * sina + textHeight / 4;
		painter->drawText(x, y, strValue);
	}

	painter->restore();
}

void GaugeArc::drawPointerCircle(QPainter* painter)
{
	int radius = 8;
	int offset = 30;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);

	painter->rotate(startAngle_);
	double degRotate = (360.0 - startAngle_ - endAngle_) / (maxValue_ - minValue_) * (value_ - minValue_);
	painter->rotate(degRotate);
	painter->drawEllipse(-radius, radius + offset, radius * 2, radius * 2);

	painter->restore();
}

void GaugeArc::drawPointerIndicator(QPainter* painter)
{
	int radius = 75;
	painter->save();
	painter->setOpacity(0.8);
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);

	QPolygon pts;
	pts.setPoints(3, -5, 0, 5, 0, 0, radius);

	painter->rotate(startAngle_);
	double degRotate = (360.0 - startAngle_ - endAngle_) / (maxValue_ - minValue_) * (value_ - minValue_);
	painter->rotate(degRotate);
	painter->drawConvexPolygon(pts);

	painter->restore();
}

void GaugeArc::drawPointerIndicatorR(QPainter* painter)
{
	int radius = 75;
	painter->save();

	QPen pen;
	pen.setWidthF(1);
	pen.setColor(pointerColor_);
	painter->setPen(pen);
	painter->setBrush(pointerColor_);

	QPolygon pts;
	pts.setPoints(3, -5, 0, 5, 0, 0, radius);

	painter->rotate(startAngle_);
	double degRotate = (360.0 - startAngle_ - endAngle_) / (maxValue_ - minValue_) * (value_ - minValue_);
	painter->rotate(degRotate);
	painter->drawConvexPolygon(pts);

	//增加绘制圆角直线,与之前三角形重叠,形成圆角指针    
	pen.setCapStyle(Qt::RoundCap);
	pen.setWidthF(4);
	painter->setPen(pen);
	painter->drawLine(0, 0, 0, radius);

	painter->restore();
}

void GaugeArc::drawPointerTriangle(QPainter* painter)
{
	int radius = 10;
	int offset = 55;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);

	QPolygon pts;
	pts.setPoints(3, -5, 0 + offset, 5, 0 + offset, 0, radius + offset);

	painter->rotate(startAngle_);
	double degRotate = (360.0 - startAngle_ - endAngle_) / (maxValue_ - minValue_) * (value_ - minValue_);
	painter->rotate(degRotate);
	painter->drawConvexPolygon(pts);

	painter->restore();
}

void GaugeArc::drawRoundCircle(QPainter* painter)
{
	int radius = 12;
	painter->save();
	painter->setOpacity(0.5);
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeArc::drawCenterCircle(QPainter* painter)
{
	int radius = 8;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeArc::drawValue(QPainter* painter)
{
	int radius = 100;
	painter->save();
	painter->setPen(textColor_);

	QFont font;
	font.setPixelSize(30);
	painter->setFont(font);

	QRectF textRect(-radius, radius / 2, radius * 2, radius / 3);
	QString strValue = QString("%1").arg((double)value_, 0, 'f', precision_);
	painter->drawText(textRect, Qt::AlignCenter, strValue);

    painter->restore();
}

const QColor &GaugeArc::scaleNumColor() const
{
    return scaleNumColor_;
}

void GaugeArc::setScaleNumColor(const QColor &newScaleNumColor)
{
    if (scaleNumColor_ == newScaleNumColor)
        return;
    scaleNumColor_ = newScaleNumColor;
    update();
}

const QColor &GaugeArc::textColor() const
{
    return textColor_;
}

void GaugeArc::setTextColor(const QColor &newTextColor)
{
    if (textColor_ == newTextColor)
        return;
    textColor_ = newTextColor;
	update();
}

const QColor &GaugeArc::pointerColor() const
{
    return pointerColor_;
}

void GaugeArc::setPointerColor(const QColor &newPointerColor)
{
    if (pointerColor_ == newPointerColor)
        return;
    pointerColor_ = newPointerColor;
	update();
}

const QColor &GaugeArc::scaleColor() const
{
    return scaleColor_;
}

void GaugeArc::setScaleColor(const QColor &newScaleColor)
{
    if (scaleColor_ == newScaleColor)
        return;
    scaleColor_ = newScaleColor;
	update();
}

const QColor &GaugeArc::arcColor() const
{
    return arcColor_;
}

void GaugeArc::setArcColor(const QColor &newArcColor)
{
    if (arcColor_ == newArcColor)
        return;
    arcColor_ = newArcColor;
	update();
}

int GaugeArc::endAngle() const
{
    return endAngle_;
}

void GaugeArc::setEndAngle(int newEndAngle)
{
    if (endAngle_ == newEndAngle)
        return;
    endAngle_ = newEndAngle;
	update();
}

int GaugeArc::startAngle() const
{
    return startAngle_;
}

void GaugeArc::setStartAngle(int newStartAngle)
{
    if (startAngle_ == newStartAngle)
        return;
    startAngle_ = newStartAngle;
	update();
}

int GaugeArc::scaleMinor() const
{
    return scaleMinor_;
}

void GaugeArc::setScaleMinor(int newScaleMinor)
{
    if (scaleMinor_ == newScaleMinor)
        return;
    scaleMinor_ = newScaleMinor;
	update();
}

int GaugeArc::scaleMajor() const
{
    return scaleMajor_;
}

void GaugeArc::setScaleMajor(int newScaleMajor)
{
    if (scaleMajor_ == newScaleMajor)
        return;
    scaleMajor_ = newScaleMajor;
	update();
}

int GaugeArc::precision() const
{
    return precision_;
}

void GaugeArc::setPrecision(int newPrecision)
{
    if (precision_ == newPrecision)
        return;
    precision_ = newPrecision;
	update();
}

double GaugeArc::value() const
{
    return value_;
}

void GaugeArc::setValue(double newValue)
{
    if (qFuzzyCompare(value_, newValue))
        return;
    value_ = newValue;
	update();
}

double GaugeArc::maxValue() const
{
    return maxValue_;
}

void GaugeArc::setMaxValue(double newMaxValue)
{
    if (qFuzzyCompare(maxValue_, newMaxValue))
        return;
    maxValue_ = newMaxValue;
	update();
}

double GaugeArc::minValue() const
{
    return minValue_;
}

void GaugeArc::setMinValue(double newMinValue)
{
    if (qFuzzyCompare(minValue_, newMinValue))
        return;
    minValue_ = newMinValue;
	update();
}

QSize GaugeArc::sizeHint() const
{
	return QSize{ 80, 80 };
}

QSize GaugeArc::minimumSizeHint() const
{
	return QSize{ 80, 80 };
}

void GaugeArc::setRange(double minValue, double maxValue)
{
	minValue_ = minValue;
	maxValue_ = maxValue;
	update();
}
