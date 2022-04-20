#include "gaugepanel.h"

#include <QtMath>
#include <QPainterPath>

GaugePanel::GaugePanel(QWidget* parent) : QWidget(parent)
{
	value_ = 20;
	hShearValue_ = 0.0;
	vShearValue_ = 0.0;
	lower_ = 0.0;
	upper_ = 30.0;
	radiusInner_ = 65.0;
	radiusOuter_ = 76.25;
	radiusHalo_ = 87.5;
	colorOuterFrame_ = QColor(50, 154, 255, 250);
	colorInnerStart_ = QColor(50, 154, 255, 180);
	colorInnerEnd_ = QColor(50, 154, 255, 70);
	colorOuterStart_ = QColor(50, 154, 255, 150);
	colorOuterEnd_ = QColor(50, 154, 255, 200);
	colorHaloStart_ = QColor(100, 180, 255, 80);
	colorHaloEnd_ = QColor(30, 80, 120, 20);

	hShearAnimation = new QPropertyAnimation(this, "hShearValue");
	vShearAnimation = new QPropertyAnimation(this, "vShearValue");
}

GaugePanel::~GaugePanel()
{
	hShearAnimation->stop();
	vShearAnimation->stop();
	delete hShearAnimation;
	delete vShearAnimation;
}

void GaugePanel::paintEvent(QPaintEvent*)
{
	const int width = this->width();
	const int height = this->height();
	const int side = qMin(width, height);

	//绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	painter.translate(width / 2, height / 2);
	painter.scale(side / 215.0, side / 215.0);

	painter.shear(double(hShearValue_ / 100.0f), double(vShearValue_ / 100.0f));

	//内层渐变
	drawInnerGradient(&painter);
	//外层渐变
	drawOuterGradient(&painter);
	//外层光晕
	drawOuterHalo(&painter);
	//刻度线
	drawScale(&painter);
	//刻度值
	drawScaleNum(&painter);
	//绘制指针
	drawPointer(&painter);
	//绘制指针扇形
	drawPointerSector(&painter);
	//绘制值
	drawValue(&painter);
	//绘制单位
	drawUnit(&painter);
}

void GaugePanel::drawOuterGradient(QPainter* painter)
{
	if (radiusHalo_ <= radiusOuter_)
		return;

	painter->save();

	QRectF rectangle(0 - radiusHalo_, 0 - radiusHalo_, radiusHalo_ * 2, radiusHalo_ * 2);
	QPen framePen(colorOuterFrame_);
	framePen.setWidthF(1.5f);
	painter->setPen(framePen);
	painter->drawEllipse(rectangle);

	painter->setPen(Qt::NoPen);

	QPainterPath smallCircle;
	QPainterPath bigCircle;

	float radius = radiusOuter_;
	smallCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);
	radius += (radiusHalo_ - radiusOuter_);
	bigCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);

	//大圆抛去小圆部分
	QPainterPath gradientPath = bigCircle - smallCircle;
	QRadialGradient gradient(0, 0, radius, 0, 0);
	//gradient.setSpread(QGradient::ReflectSpread);

	gradient.setColorAt(0.85, colorOuterStart_);
	gradient.setColorAt(0.98, colorOuterEnd_);
	painter->setBrush(gradient);
	painter->drawPath(gradientPath);

	painter->restore();
}

void GaugePanel::drawInnerGradient(QPainter* painter)
{
	if (radiusOuter_ <= radiusInner_)
		return;

	painter->save();
	painter->setPen(Qt::NoPen);

	QPainterPath smallCircle;
	QPainterPath bigCircle;

	float radius = radiusInner_;
	smallCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);
	radius += (radiusOuter_ - radiusInner_);
	bigCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);

	//大圆抛去小圆部分
	QPainterPath gradientPath = bigCircle - smallCircle;
	QRadialGradient gradient(0, 0, radius, 0, 0);
	//gradient.setSpread(QGradient::ReflectSpread);

	gradient.setColorAt(0.7, colorInnerStart_);
	gradient.setColorAt(1, colorInnerEnd_);
	painter->setBrush(gradient);
	painter->drawPath(gradientPath);

	painter->restore();
}

void GaugePanel::drawOuterHalo(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);

	QPainterPath smallCircle;
	QPainterPath bigCircle;

	float radius = radiusHalo_;
	smallCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);
	radius += (100.0 - radiusHalo_);
	bigCircle.addEllipse(-radius, -radius, radius * 2, radius * 2);

	//大圆抛去小圆部分
	QPainterPath gradientPath = bigCircle - smallCircle;
	QRadialGradient gradient(0, 0, 100, 0, 0);
	gradient.setSpread(QGradient::ReflectSpread);

	gradient.setColorAt(radiusHalo_ / 100, colorHaloStart_);
	gradient.setColorAt(1, colorHaloEnd_);
	painter->setBrush(gradient);
	painter->drawPath(gradientPath);

	painter->restore();
}

void GaugePanel::drawScale(QPainter* painter)
{
	float radius = 85;
	painter->save();
	painter->setPen(QColor(255, 255, 255));

	painter->rotate(30);
	int steps = (30);
	double angleStep = (360.0 - 60) / steps;
	QPen pen = painter->pen();
	pen.setCapStyle(Qt::RoundCap);

	for (int i = 0; i <= steps; i++) {
		if (i % 3 == 0) {
			pen.setWidthF(1.5);
			painter->setPen(pen);
			QLineF line(0.0f, radius - 8.0f, 0.0f, radius);
			painter->drawLine(line);
		}
		else {
			pen.setWidthF(0.5);
			painter->setPen(pen);
			QLineF line(0.0f, radius - 3.0f, 0.0f, radius);
			painter->drawLine(line);
		}

		painter->rotate(angleStep);
	}

	painter->restore();
}

void GaugePanel::drawScaleNum(QPainter* painter)
{
	float radius = 95.0f;
	painter->save();
	painter->setPen(QColor(255, 255, 255));

	double startRad = (330 - 90) * (M_PI / 180);
	double deltaRad = (300) * (M_PI / 180) / 10;

	for (int i = 0; i <= 10; i++) {
		double sina = sin(startRad - i * deltaRad);
		double cosa = cos(startRad - i * deltaRad);
		double value = lower_ + i * (upper_ - lower_) / 10.0;//刻度值范围

		QString strValue = QString("%1").arg((double)value, 0, 'f', 0);
		//double textWidth = fontMetrics().width(strValue);
		//double textHeight = fontMetrics().height();
		const auto rect = fontMetrics().boundingRect(strValue);
		double textWidth = rect.width();
		double textHeight = rect.height();
		int x = radius * cosa - textWidth / 2;
		int y = -radius * sina + textHeight / 4;
		painter->drawText(x, y, strValue);
	}

	painter->restore();
}

void GaugePanel::drawPointer(QPainter* painter)
{
	painter->save();
	// 默认向下画线，需要顺时针旋转角度
	float radius = 83.0;
	double degrees = 30 + (value_ - lower_) / (upper_ - lower_) * 300;//下方有60度区域为空
	painter->rotate(degrees);
	QPen pen = painter->pen();
	pen.setWidthF(1.0);
	pen.setColor(QColor(50, 154, 255, 200));
	painter->setPen(pen);
	QLineF line(0.0f, 0.0f, 0.0f, radius);
	painter->drawLine(line);

	painter->restore();
}

void GaugePanel::drawPointerSector(QPainter* painter)
{
	float radius = 87.5f;
	painter->save();
	painter->setPen(Qt::NoPen);

	const int spanAngle = (value_ - lower_) / (upper_ - lower_) * 300 * 16;
	QRectF rect(-radius, -radius, radius * 2, radius * 2);
	painter->setBrush(QColor(50, 154, 255, 50));
	painter->drawPie(rect, -120 * 16, -spanAngle);

	painter->restore();
}

void GaugePanel::drawValue(QPainter* painter)
{
	int radius = 100;
	painter->save();
	painter->setPen(QColor(255, 255, 255));
	painter->setFont(QFont("Arial", 22, 22, true));

	QRectF textRect(-radius, -radius, radius * 2, radius * 2);
	QString strValue = QString("%1").arg((double)value_, 0, 'f', 0);
	painter->drawText(textRect, Qt::AlignCenter, strValue);

	painter->restore();
}

void GaugePanel::drawUnit(QPainter* painter)
{
	int radius = 100;
	painter->save();
	painter->setPen(QColor(255, 255, 255));
	painter->setFont(QFont("Arial", 9, -1, true));

	QRectF textRect(-radius, -radius + 20, radius * 2, radius * 2);
	painter->drawText(textRect, Qt::AlignCenter, "km/h");

	painter->restore();
}

double GaugePanel::getValue() const
{
	return this->value_;
}

double GaugePanel::getLower() const
{
	return lower_;
}

double GaugePanel::getUpper() const
{
	return upper_;
}

int GaugePanel::getHShearValue() const
{
	return this->hShearValue_;
}

int GaugePanel::getVShearValue() const
{
	return this->vShearValue_;
}

double GaugePanel::getRadiusInner() const
{
	return radiusInner_;
}

double GaugePanel::getRadiusOuter() const
{
	return radiusOuter_;
}

double GaugePanel::getRadiusHalo() const
{
	return radiusHalo_;
}

QColor GaugePanel::getColorOuterFrame() const
{
	return colorOuterFrame_;
}

QColor GaugePanel::getColorInnerStart() const
{
	return colorInnerStart_;
}

QColor GaugePanel::getColorInnerEnd() const
{
	return colorInnerEnd_;
}

QColor GaugePanel::getColorOuterStart() const
{
	return colorOuterStart_;
}

QColor GaugePanel::getColorOuterEnd() const
{
	return colorOuterEnd_;
}

QColor GaugePanel::getColorHaloStart() const
{
	return colorHaloStart_;
}

QColor GaugePanel::getColorHaloEnd() const
{
	return colorHaloEnd_;
}

void GaugePanel::setValue(int value)
{
	setValue(double(value));
}

void GaugePanel::setValue(double value) {
	updateValue(value);
}

void GaugePanel::setLower(double value)
{
	lower_ = value;
	update();
}

void GaugePanel::setUpper(double value)
{
	upper_ = value;
	update();
}

void GaugePanel::setValueRange(double lower, double upper)
{
	lower_ = lower;
	upper_ = upper;
	update();
}

void GaugePanel::setHShearValue(int value)
{
	if (value > 100 || value < -100)
		return;

	this->hShearValue_ = value;
	update();
}

void GaugePanel::setVShearValue(int value)
{
	if (value > 100 || value < -100)
		return;

	this->vShearValue_ = value;
	update();
}

void GaugePanel::setColorOuterFrame(QColor color)
{
	colorOuterFrame_ = color;
}

void GaugePanel::setRadiusInner(int radius)
{
	setRadiusInner(double(radius));
}

void GaugePanel::setRadiusInner(double radius)
{
	if (radius >= 0.0f && radius < 100.0f) {
		radiusInner_ = radius;
		update();
	}
}

void GaugePanel::setRadiusOuter(int radius)
{
	setRadiusOuter(double(radius));
}

void GaugePanel::setRadiusOuter(double radius)
{
	if (radius > 0.0f && radius < 100.0f) {
		radiusOuter_ = radius;
		update();
	}
}

void GaugePanel::setRadiusHalo(int radius)
{
	setRadiusHalo(double(radius));
}

void GaugePanel::setRadiusHalo(double radius)
{
	if (radius > 0.0f && radius < 100.0f) {
		radiusHalo_ = radius;
		update();
	}
}

void GaugePanel::setColorInnerStart(QColor color)
{
	colorInnerStart_ = color;
}

void GaugePanel::setColorInnerEnd(QColor color)
{
	colorInnerEnd_ = color;
}

void GaugePanel::setColorOuterStart(QColor color)
{
	colorOuterStart_ = color;
}

void GaugePanel::setColorOuterEnd(QColor color)
{
	colorOuterEnd_ = color;
}

void GaugePanel::setColorHaloStart(QColor color)
{
	colorHaloStart_ = color;
}

void GaugePanel::setColorHaloEnd(QColor color)
{
	colorHaloEnd_ = color;
}

void GaugePanel::startShearAnimal(int duration, int hShearValue, int vShearValue)
{
	if (hShearValue == this->hShearValue_ && vShearValue == this->vShearValue_) {
		return;
	}

	if (hShearAnimation->state() != QPropertyAnimation::Stopped) {
		hShearAnimation->stop();
	}

	if (vShearAnimation->state() != QPropertyAnimation::Stopped) {
		vShearAnimation->stop();
	}

	hShearAnimation->setDuration(duration);
	hShearAnimation->setStartValue(this->hShearValue_);
	hShearAnimation->setEndValue(hShearValue);
	hShearAnimation->start();

	vShearAnimation->setDuration(duration);
	vShearAnimation->setStartValue(this->vShearValue_);
	vShearAnimation->setEndValue(vShearValue);
	vShearAnimation->start();
}

void GaugePanel::updateValue(double value)
{
	if (value < lower_ || value > upper_) {
		return;
	}

	value_ = value;
	update();
}

