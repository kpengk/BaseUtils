#include "gaugecar.h"
#include <QPainter>
#include <QPainterPath>

GaugeCar::GaugeCar(QWidget* parent)
	:QWidget(parent)
{
	minValue_ = 0.0;
	maxValue_ = 100.0;
	value_ = 85.2;
	precision_ = 0;

	scaleMajor_ = 10;
	scaleMinor_ = 5;
	startAngle_ = 45;
	endAngle_ = 45;

	outerCircleColor_ = QColor(80, 80, 80);
	innerCircleColor_ = QColor(60, 60, 60);

	pieColorStart_ = QColor(23, 190, 155);
	pieColorMid_ = QColor(218, 218, 0);
	pieColorEnd_ = QColor(255, 103, 104);

	coverCircleColor_ = QColor(100, 100, 100);
	scaleColor_ = QColor(253, 253, 253);
	pointerColor_ = QColor(222, 103, 104);
	centerCircleColor_ = QColor(246, 252, 248);
	textColor_ = QColor(2, 4, 3);

	showOverlay_ = true;
	overlayColor_ = QColor(139, 139, 139);

	pieStyle_ = PieStyle_Current;
	pointerStyle_ = PointerStyle_Indicator;
}

GaugeCar::~GaugeCar()
{

}

QSize GaugeCar::sizeHint() const
{
	return QSize{ 80, 80 };
}

QSize GaugeCar::minimumSizeHint() const
{
	return QSize{ 80, 80 };
}

void GaugeCar::paintEvent(QPaintEvent*)
{
	int width = this->width();
	int height = this->height();
	int side = qMin(width, height);

	//绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.translate(width / 2, height / 2);
	painter.scale(side / 200.0, side / 200.0);

	//绘制外圆
	drawOuterCircle(&painter);
	//绘制内圆
	drawInnerCircle(&painter);
	//绘制饼圆
	drawColorPie(&painter);
	//绘制覆盖圆 用以遮住饼圆多余部分
	drawCoverCircle(&painter);
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
	//绘制遮罩层
	drawOverlay(&painter);
}

void GaugeCar::drawOuterCircle(QPainter* painter)
{
	int radius = 99;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(outerCircleColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeCar::drawInnerCircle(QPainter* painter)
{
	int radius = 90;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(innerCircleColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeCar::drawColorPie(QPainter* painter)
{
	int radius = 60;
	painter->save();
	painter->setPen(Qt::NoPen);

	QRectF rect(-radius, -radius, radius * 2, radius * 2);

	if (pieStyle_ == PieStyle_Three) {
		//计算总范围角度,根据占比例自动计算三色圆环范围角度
		//可以更改比例
		double angleAll = 360.0 - startAngle_ - endAngle_;
		double angleStart = angleAll * 0.7;
		double angleMid = angleAll * 0.15;
		double angleEnd = angleAll * 0.15;

		//增加偏移量使得看起来没有脱节
		int offset = 3;

		//绘制开始饼圆
		painter->setBrush(pieColorStart_);
		painter->drawPie(rect, (270 - startAngle_ - angleStart) * 16, angleStart * 16);

		//绘制中间饼圆
		painter->setBrush(pieColorMid_);
		painter->drawPie(rect, (270 - startAngle_ - angleStart - angleMid) * 16 + offset, angleMid * 16);

		//绘制结束饼圆
		painter->setBrush(pieColorEnd_);
		painter->drawPie(rect, (270 - startAngle_ - angleStart - angleMid - angleEnd) * 16 + offset * 2, angleEnd * 16);
	}
	else if (pieStyle_ == PieStyle_Current) {
		//计算总范围角度,当前值范围角度,剩余值范围角度
		double angleAll = 360.0 - startAngle_ - endAngle_;
		double angleCurrent = angleAll * ((value_ - minValue_) / (maxValue_ - minValue_));
		double angleOther = angleAll - angleCurrent;

		//绘制当前值饼圆
		painter->setBrush(pieColorStart_);
		painter->drawPie(rect, (270 - startAngle_ - angleCurrent) * 16, angleCurrent * 16);

		//绘制剩余值饼圆
		painter->setBrush(pieColorEnd_);
		painter->drawPie(rect, (270 - startAngle_ - angleCurrent - angleOther) * 16, angleOther * 16);
	}

	painter->restore();
}

void GaugeCar::drawCoverCircle(QPainter* painter)
{
	int radius = 50;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(coverCircleColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeCar::drawScale(QPainter* painter)
{
	int radius = 72;
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
			painter->drawLine(0, radius - 10, 0, radius);
		}
		else {
			pen.setWidthF(0.5);
			painter->setPen(pen);
			painter->drawLine(0, radius - 5, 0, radius);
		}

		painter->rotate(angleStep);
	}

	painter->restore();
}

void GaugeCar::drawScaleNum(QPainter* painter)
{
	int radius = 82;
	painter->save();
	painter->setPen(scaleColor_);

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

void GaugeCar::drawPointerCircle(QPainter* painter)
{
	int radius = 6;
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

void GaugeCar::drawPointerIndicator(QPainter* painter)
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

void GaugeCar::drawPointerIndicatorR(QPainter* painter)
{
	int radius = 75;
	painter->save();
	painter->setOpacity(1.0);

	QPen pen;
	pen.setWidth(1);
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

void GaugeCar::drawPointerTriangle(QPainter* painter)
{
	int radius = 10;
	int offset = 38;
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

void GaugeCar::drawRoundCircle(QPainter* painter)
{
	int radius = 18;
	painter->save();
	painter->setOpacity(0.8);
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeCar::drawCenterCircle(QPainter* painter)
{
	int radius = 15;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(centerCircleColor_);
	painter->drawEllipse(-radius, -radius, radius * 2, radius * 2);
	painter->restore();
}

void GaugeCar::drawValue(QPainter* painter)
{
	int radius = 100;
	painter->save();
	painter->setPen(textColor_);

	QFont font;
	font.setPixelSize(18);
	painter->setFont(font);

	QRectF textRect(-radius, -radius, radius * 2, radius * 2);
	QString strValue = QString("%1").arg((double)value_, 0, 'f', precision_);
	painter->drawText(textRect, Qt::AlignCenter, strValue);

	painter->restore();
}

void GaugeCar::drawOverlay(QPainter* painter)
{
	if (!showOverlay_) {
		return;
	}

	int radius = 90;
	painter->save();
	painter->setPen(Qt::NoPen);

	QPainterPath smallCircle;
	QPainterPath bigCircle;
	radius -= 1;
	smallCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);
	radius *= 2;
	bigCircle.addEllipse(-radius, -radius + 140, radius * 2, radius * 2);

	//高光的形状为小圆扣掉大圆的部分
	QPainterPath highlight = smallCircle - bigCircle;

	QLinearGradient linearGradient(0, -radius / 2, 0, 0);
	overlayColor_.setAlpha(100);
	linearGradient.setColorAt(0.0, overlayColor_);
	overlayColor_.setAlpha(30);
	linearGradient.setColorAt(1.0, overlayColor_);
	painter->setBrush(linearGradient);
	painter->rotate(-20);
	painter->drawPath(highlight);

    painter->restore();
}

void GaugeCar::setRange(double minValue, double maxValue)
{
	setMinValue(minValue);
	setMaxValue(maxValue);
}

GaugeCar::PointerStyle GaugeCar::pointerStyle() const
{
    return pointerStyle_;
}

void GaugeCar::setPointerStyle(PointerStyle newPointerStyle)
{
    if (pointerStyle_ == newPointerStyle)
        return;
    pointerStyle_ = newPointerStyle;
	update();
}

GaugeCar::PieStyle GaugeCar::pieStyle() const
{
    return pieStyle_;
}

void GaugeCar::setPieStyle(PieStyle newPieStyle)
{
    if (pieStyle_ == newPieStyle)
        return;
    pieStyle_ = newPieStyle;
	update();
}

const QColor &GaugeCar::overlayColor() const
{
    return overlayColor_;
}

void GaugeCar::setOverlayColor(const QColor &newOverlayColor)
{
    if (overlayColor_ == newOverlayColor)
        return;
    overlayColor_ = newOverlayColor;
	update();
}

bool GaugeCar::showOverlay() const
{
    return showOverlay_;
}

void GaugeCar::setShowOverlay(bool newShowOverlay)
{
    if (showOverlay_ == newShowOverlay)
        return;
    showOverlay_ = newShowOverlay;
	update();
}

const QColor &GaugeCar::textColor() const
{
    return textColor_;
}

void GaugeCar::setTextColor(const QColor &newTextColor)
{
    if (textColor_ == newTextColor)
        return;
    textColor_ = newTextColor;
	update();
}

const QColor &GaugeCar::centerCircleColor() const
{
    return centerCircleColor_;
}

void GaugeCar::setCenterCircleColor(const QColor &newCenterCircleColor)
{
    if (centerCircleColor_ == newCenterCircleColor)
        return;
    centerCircleColor_ = newCenterCircleColor;
	update();
}

const QColor &GaugeCar::pointerColor() const
{
    return pointerColor_;
}

void GaugeCar::setPointerColor(const QColor &newPointerColor)
{
    if (pointerColor_ == newPointerColor)
        return;
    pointerColor_ = newPointerColor;
	update();
}

const QColor &GaugeCar::scaleColor() const
{
    return scaleColor_;
}

void GaugeCar::setScaleColor(const QColor &newScaleColor)
{
    if (scaleColor_ == newScaleColor)
        return;
    scaleColor_ = newScaleColor;
	update();
}

const QColor &GaugeCar::coverCircleColor() const
{
    return coverCircleColor_;
}

void GaugeCar::setCoverCircleColor(const QColor &newCoverCircleColor)
{
    if (coverCircleColor_ == newCoverCircleColor)
        return;
    coverCircleColor_ = newCoverCircleColor;
	update();
}

const QColor &GaugeCar::pieColorEnd() const
{
    return pieColorEnd_;
}

void GaugeCar::setPieColorEnd(const QColor &newPieColorEnd)
{
    if (pieColorEnd_ == newPieColorEnd)
        return;
    pieColorEnd_ = newPieColorEnd;
	update();
}

const QColor &GaugeCar::pieColorMid() const
{
    return pieColorMid_;
}

void GaugeCar::setPieColorMid(const QColor &newPieColorMid)
{
    if (pieColorMid_ == newPieColorMid)
        return;
    pieColorMid_ = newPieColorMid;
	update();
}

const QColor &GaugeCar::pieColorStart() const
{
    return pieColorStart_;
}

void GaugeCar::setPieColorStart(const QColor &newPieColorStart)
{
    if (pieColorStart_ == newPieColorStart)
        return;
    pieColorStart_ = newPieColorStart;
	update();
}

const QColor &GaugeCar::innerCircleColor() const
{
    return innerCircleColor_;
}

void GaugeCar::setInnerCircleColor(const QColor &newInnerCircleColor)
{
    if (innerCircleColor_ == newInnerCircleColor)
        return;
    innerCircleColor_ = newInnerCircleColor;
	update();
}

const QColor &GaugeCar::outerCircleColor() const
{
    return outerCircleColor_;
}

void GaugeCar::setOuterCircleColor(const QColor &newOuterCircleColor)
{
    if (outerCircleColor_ == newOuterCircleColor)
        return;
    outerCircleColor_ = newOuterCircleColor;
	update();
}

int GaugeCar::endAngle() const
{
    return endAngle_;
}

void GaugeCar::setEndAngle(int newEndAngle)
{
    if (endAngle_ == newEndAngle)
        return;
    endAngle_ = newEndAngle;
	update();
}

int GaugeCar::startAngle() const
{
    return startAngle_;
}

void GaugeCar::setStartAngle(int newStartAngle)
{
    if (startAngle_ == newStartAngle)
        return;
    startAngle_ = newStartAngle;
	update();
}

int GaugeCar::scaleMinor() const
{
    return scaleMinor_;
}

void GaugeCar::setScaleMinor(int newScaleMinor)
{
    if (scaleMinor_ == newScaleMinor)
        return;
    scaleMinor_ = newScaleMinor;
	update();
}

int GaugeCar::scaleMajor() const
{
    return scaleMajor_;
}

void GaugeCar::setScaleMajor(int newScaleMajor)
{
    if (scaleMajor_ == newScaleMajor)
        return;
    scaleMajor_ = newScaleMajor;
	update();
}

int GaugeCar::precision() const
{
    return precision_;
}

void GaugeCar::setPrecision(int newPrecision)
{
    if (precision_ == newPrecision)
        return;
    precision_ = newPrecision;
	update();
}

double GaugeCar::value() const
{
    return value_;
}

void GaugeCar::setValue(double newValue)
{
    if (qFuzzyCompare(value_, newValue))
        return;
    value_ = newValue;
	update();
}

double GaugeCar::maxValue() const
{
    return maxValue_;
}

void GaugeCar::setMaxValue(double newMaxValue)
{
    if (qFuzzyCompare(maxValue_, newMaxValue))
        return;
    maxValue_ = newMaxValue;
	update();
}

double GaugeCar::minValue() const
{
    return minValue_;
}

void GaugeCar::setMinValue(double newMinValue)
{
    if (qFuzzyCompare(minValue_, newMinValue))
        return;
    minValue_ = newMinValue;
	update();
}
