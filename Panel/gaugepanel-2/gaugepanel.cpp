#include "gaugepanel.h"
#include <QPainter>

GaugePanel::GaugePanel(QWidget* parent)
	:QWidget(parent)
{
	minValue_ = 0.0;
	maxValue_ = 60.0;
	value_ = 38.2;
	precision_ = 1;
	scalePrecision_ = 0;

	scaleMajor_ = 10;
	scaleMinor_ = 5;
	startAngle_ = 45;
	endAngle_ = 45;

	ringWidth_ = 8;
	ringColorStops_ = {
		{ 0.5, QColor(103, 224, 227) },
		{ 0.7, QColor(250, 201, 86) },
		{ 1.0, QColor(253, 102, 109) }
	};

	scaleColor_ = QColor(4, 159, 133);
	pointerColor_ = QColor(60, 161, 217);
	bgColor_ = QColor(34, 41, 56);
	textColor_ = QColor(228, 233, 233);

	unit_ = "km/h";
	text_ = "Speed";
}

GaugePanel::~GaugePanel()
{

}

double GaugePanel::getMinValue() const
{
	return minValue_;
}

double GaugePanel::getMaxValue() const
{
	return maxValue_;
}

double GaugePanel::getValue() const
{
	return value_;
}

int GaugePanel::getPrecision() const
{
	return precision_;
}

int GaugePanel::getScalePrecision() const
{
	return scalePrecision_;
}

int GaugePanel::getScaleMajor() const
{
	return scaleMajor_;
}

int GaugePanel::getScaleMinor() const
{
	return scaleMinor_;
}

int GaugePanel::getStartAngle() const
{
	return startAngle_;
}

int GaugePanel::getEndAngle() const
{
	return endAngle_;
}

int GaugePanel::getRingWidth() const
{
	return ringWidth_;
}

const QMap<double, QColor>& GaugePanel::getRingColor() const
{
	return ringColorStops_;
}

const QColor& GaugePanel::getScaleColor() const
{
	return scaleColor_;
}

const QColor& GaugePanel::getPointerColor() const
{
	return pointerColor_;
}

const QColor& GaugePanel::getBgColor() const
{
	return bgColor_;
}

const QColor& GaugePanel::getTextColor() const
{
	return textColor_;
}

const QString& GaugePanel::getUnit() const
{
	return unit_;
}

const QString& GaugePanel::getText() const
{
	return text_;
}

QSize GaugePanel::sizeHint() const
{
	return QSize{ 80, 80 };
}

QSize GaugePanel::minimumSizeHint() const
{
	return QSize{ 80, 80 };
}

void GaugePanel::setRange(double minValue, double maxValue)
{
	minValue_ = minValue;
	maxValue_ = maxValue;
	update();
}

void GaugePanel::setMinValue(double minValue)
{
	minValue_ = minValue;
	update();
}

void GaugePanel::setMaxValue(double maxValue)
{
	maxValue_ = maxValue;
	update();
}

void GaugePanel::setValue(double value)
{
	value_ = value;
	update();
}

void GaugePanel::setPrecision(int precision)
{
	precision_ = precision;
	update();
}

void GaugePanel::setScalePrecision(int scalePrecision)
{
	scalePrecision_ = scalePrecision;
	update();
}

void GaugePanel::setScaleMajor(int scaleMajor)
{
	scaleMajor_ = scaleMajor;
	update();
}

void GaugePanel::setScaleMinor(int scaleMinor)
{
	scaleMinor_ = scaleMinor;
	update();
}

void GaugePanel::setStartAngle(int startAngle)
{
	startAngle_ = startAngle;
	update();
}

void GaugePanel::setEndAngle(int endAngle)
{
	endAngle_ = endAngle;
	update();
}

void GaugePanel::setRingWidth(int ringWidth)
{
	ringWidth_ = ringWidth;
	update();
}

void GaugePanel::setRingColor(const QMap<double, QColor>& ringColorStops)
{
	ringColorStops_ = ringColorStops;
	update();
}

void GaugePanel::setScaleColor(const QColor& scaleColor)
{
	scaleColor_ = scaleColor;
	update();
}

void GaugePanel::setPointerColor(const QColor& pointerColor)
{
	pointerColor_ = pointerColor;
	update();
}

void GaugePanel::setBgColor(const QColor& bgColor)
{
	bgColor_ = bgColor;
	update();
}

void GaugePanel::setTextColor(const QColor& textColor)
{
	textColor_ = textColor;
	update();
}

void GaugePanel::setUnit(const QString& unit)
{
	unit_ = unit;
	update();
}

void GaugePanel::setText(const QString& text)
{
	text_ = text;
	update();
}

void GaugePanel::paintEvent(QPaintEvent*)
{
	const int width = this->width();
	const int height = this->height();
	const int side = qMin(width, height) - 2;
	constexpr int padding = 4;

	//绘制准备工作,启用反锯齿,平移坐标轴中心,等比例缩放
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	//绘制背景
	if (bgColor_ != Qt::transparent) {
		painter.setPen(Qt::NoPen);
		painter.fillRect(this->rect(), bgColor_);
	}

	//平移缩放，确保绘制区域为RectF(-100, -100, 100, 100)，且以窗口中心作为原点
	painter.translate(width / 2, height / 2);
	painter.scale(side / (200.0 + padding), side / (200.0 + padding));

	//绘制圆环
	drawRing(&painter);
	//绘制刻度线
	drawScale(&painter);
	//绘制刻度值
	drawScaleNum(&painter);
	//绘制指示器
	drawPointer(&painter);
	//绘制当前值
	drawValue(&painter);
}

void GaugePanel::drawRing(QPainter* painter)
{
	int radius = 70;
	painter->save();

	QPen pen;
	pen.setCapStyle(Qt::FlatCap);
	pen.setWidthF(ringWidth_);

	radius = radius - ringWidth_;
	QRectF rect = QRectF(-radius, -radius, radius * 2, radius * 2);
	const double angleAll = 360.0 - startAngle_ - endAngle_;

	double endAngle = 270.0 - startAngle_;
	for (auto iter = ringColorStops_.constBegin(); iter != ringColorStops_.constEnd(); ++iter)
	{
		const double startAngle = 270.0 - startAngle_ - angleAll * iter.key();

		pen.setColor(iter.value());
		painter->setPen(pen);
		painter->drawArc(rect, startAngle * 16, (endAngle - startAngle) * 16);
		endAngle -= endAngle - startAngle;
	}
	painter->restore();
}

void GaugePanel::drawScale(QPainter* painter)
{
	constexpr int radius = 80;
	painter->save();

	painter->rotate(startAngle_);
	const int steps = scaleMajor_ * scaleMinor_;
	const double angleStep = (360.0 - startAngle_ - endAngle_) / steps;

	QPen pen;
	pen.setCapStyle(Qt::RoundCap);
	pen.setColor(scaleColor_);

	for (int i = 0; i <= steps; i++) {
		if (i % scaleMinor_ == 0) {
			pen.setWidthF(1.5);
			painter->setPen(pen);
			painter->drawLine(0, radius - 8, 0, radius + 3);
		}
		else {
			pen.setWidthF(0.5);
			painter->setPen(pen);
			painter->drawLine(0, radius - 8, 0, radius - 3);
		}

		painter->rotate(angleStep);
	}

	painter->restore();
}

void GaugePanel::drawScaleNum(QPainter* painter)
{
	const int radius = 95;
	painter->save();
	painter->setPen(scaleColor_);
	//painter->setFont(QFont("Microsoft YaHei UI", 10));

	const double startRad = qDegreesToRadians(360 - startAngle_ - 90);
	const double deltaRad = qDegreesToRadians(360 - startAngle_ - endAngle_) / scaleMajor_;

	for (int i = 0; i <= scaleMajor_; i++) {
		double sina = qSin(startRad - i * deltaRad);
		double cosa = qCos(startRad - i * deltaRad);
		double value = i * ((maxValue_ - minValue_) / scaleMajor_) + minValue_;

		QString strValue = QString("%1").arg((double)value, 0, 'f', scalePrecision_);
		const QRect rect = fontMetrics().boundingRect(strValue);
		const double textWidth = rect.width();
		const double textHeight = rect.height();
		const int x = radius * cosa - textWidth / 2;
		const int y = -radius * sina + textHeight / 4;
		painter->drawText(x, y, strValue);
	}

	painter->restore();
}

void GaugePanel::drawPointer(QPainter* painter)
{
	const int radius = 70;
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(pointerColor_);

	QPolygon pts;
	pts.setPoints(4, -5, 0, 0, -8, 5, 0, 0, radius);

	painter->rotate(startAngle_);
	const double degRotate = (360.0 - startAngle_ - endAngle_) / (maxValue_ - minValue_) * (value_ - minValue_);
	painter->rotate(degRotate);
	painter->drawConvexPolygon(pts);

	painter->restore();
}

void GaugePanel::drawValue(QPainter* painter)
{
	int radius = 100;
	painter->save();
	painter->setPen(textColor_);

	QFont font;
	font.setPixelSize(15);
	painter->setFont(font);

	QString strValue = QString("%1").arg((double)value_, 0, 'f', precision_);
	strValue = QString("%1 %2").arg(strValue).arg(unit_);
	QRectF valueRect(-radius, radius / 3.5, radius * 2, radius / 3.5);
	painter->drawText(valueRect, Qt::AlignCenter, strValue);

	QRectF textRect(-radius, radius / 2.5, radius * 2, radius / 2.5);
	font.setPixelSize(12);
	painter->setFont(font);
	painter->drawText(textRect, Qt::AlignCenter, text_);

	painter->restore();
}
