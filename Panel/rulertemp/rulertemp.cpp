#include "rulertemp.h"
#include <QPainter>
#include <QPainterPath>

RulerTemp::RulerTemp(QWidget* parent)
{
	minValue_ = 0.0;
	maxValue_ = 100.0;
	value_ = 70.2;

	precision_ = 0;
	longStep_ = 10;
	shortStep_ = 1;
	space_ = 1;

	showUserValue_ = false;
	userValue_ = 60;
	userValueColor_ = QColor(255, 153, 164);

	bgColorStart_ = QColor(58, 58, 58);
	bgColorEnd_ = QColor(98, 98, 98);
	lineColor_ = QColor(205, 205, 205);
	barBgColor_ = QColor(229, 229, 229);
	barColor_ = QColor(254, 107, 107);

	barPosition_ = BarPosition::BarPosition_Left;
	tickPosition_ = TickPosition::TickPosition_Both;

	barWidth_ = 12;
	barHeight_ = 0;
	radius_ = 22;
	targetX_ = 0;
}

RulerTemp::~RulerTemp()
{
}

void RulerTemp::resizeEvent(QResizeEvent*)
{
	barHeight_ = this->height() - radius_ * 2;
	targetX_ = this->width() / 2;
}

void RulerTemp::paintEvent(QPaintEvent*)
{
	//绘制准备工作,启用反锯齿
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	//绘制背景
	drawBg(&painter);

	//绘制标尺及刻度尺
	if (tickPosition_ == TickPosition_Left) {
		drawRuler(&painter, 0);
	}
	else if (tickPosition_ == TickPosition_Right) {
		drawRuler(&painter, 1);
	}
	else if (tickPosition_ == TickPosition_Both) {
		drawRuler(&painter, 0);
		drawRuler(&painter, 1);
	}

	//绘制水银柱背景,包含水银柱底部圆
	drawBarBg(&painter);

	//绘制当前水银柱,包含水银柱底部圆
	drawBar(&painter);

	//绘制当前值
	drawValue(&painter);
}

void RulerTemp::drawBg(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	QLinearGradient bgGradient(QPointF(0, 0), QPointF(0, height()));
	bgGradient.setColorAt(0.0, bgColorStart_);
	bgGradient.setColorAt(1.0, bgColorEnd_);
	painter->setBrush(bgGradient);
	painter->drawRect(rect());
	painter->restore();
}

void RulerTemp::drawBarBg(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(barBgColor_);

	const int barX = targetX_ - barWidth_ / 2;
	const int barY = space_;
	QRectF barRect(barX, barY, barWidth_, barHeight_);

	const int circleX = targetX_ - radius_;
	//偏移 2 个像素,使得看起来边缘完整
	const int circleY = height() - radius_ * 2 - 2;
	const int circleWidth = radius_ * 2;
	const QRectF circleRect(circleX, circleY, circleWidth, circleWidth);

	QPainterPath path;
	path.addRect(barRect);
	path.addEllipse(circleRect);
	path.setFillRule(Qt::WindingFill);
	painter->drawPath(path);
	painter->restore();
}

void RulerTemp::drawRuler(QPainter* painter, int type)
{
	painter->save();
	painter->setPen(lineColor_);

	int barPercent = barWidth_ / 8;

	if (barPercent < 2) {
		barPercent = 2;
	}

	//绘制纵向标尺刻度
	const double length = height() - 2 * space_ - 2 * radius_;
	//计算每一格移动多少
	const double increment = length / (maxValue_ - minValue_);

	//长线条短线条长度
	int longLineLen = 10;
	int shortLineLen = 7;

	//绘制纵向标尺线 偏移 5 像素
	int offset = barWidth_ / 2 + 5;

	//左侧刻度尺需要重新计算
	if (type == 0) {
		offset = -offset;
		longLineLen = -longLineLen;
		shortLineLen = -shortLineLen;
	}

	const double initX = targetX_ + offset;
	double initY = space_ + barPercent;
	const QPointF topPot(initX, initY);
	const QPointF bottomPot(initX, height() - 2 * radius_ - 5);
	painter->drawLine(topPot, bottomPot);

	//根据范围值绘制刻度值及刻度值
	for (int i = maxValue_; i >= minValue_; i = i - shortStep_) {
		if (i % longStep_ == 0) {
			//绘制长线条
			const QPointF leftPot(initX + longLineLen, initY);
			const QPointF rightPot(initX, initY);
			painter->drawLine(leftPot, rightPot);

			//绘制文字
			QString strValue = QString("%1").arg((double)i, 0, 'f', precision_);
			double fontHeight = painter->fontMetrics().height();

			if (type == 0) {
				QRect textRect(initX - 45, initY - fontHeight / 3, 30, 15);
				painter->drawText(textRect, Qt::AlignRight, strValue);
			}
			else if (type == 1) {
				QRect textRect(initX + longLineLen + 5, initY - fontHeight / 3, 30, 15);
				painter->drawText(textRect, Qt::AlignLeft, strValue);
			}
		}
		else {
			//绘制短线条
			QPointF leftPot(initX + shortLineLen, initY);
			QPointF rightPot(initX, initY);
			painter->drawLine(leftPot, rightPot);
		}

		initY += increment * shortStep_;
	}

	painter->restore();
}

void RulerTemp::drawBar(QPainter* painter)
{
	painter->save();
	painter->setPen(Qt::NoPen);
	painter->setBrush(barColor_);

	//计算在背景宽度的基础上缩小的百分比, 至少为 2
	const int barPercent = std::max(barWidth_ / 8, 2);
	const int circlePercent = std::max(radius_ / 6, 2);

	//标尺刻度高度
	double length = height() - 2 * space_ - 2 * radius_;
	//计算每一格移动多少
	const double increment = length / (maxValue_ - minValue_);
	//计算标尺的高度
	const int rulerHeight = height() - 1 * space_ - 2 * radius_;

	const int barX = targetX_ - barWidth_ / 2;
	const int barY = rulerHeight - (value_ - minValue_) * increment;
	barRect_ = QRectF(barX + barPercent, barY + barPercent, barWidth_ - barPercent * 2, barHeight_ + radius_ - barY);

	const int circleX = targetX_ - radius_;
	//偏移 2 个像素,使得看起来边缘完整
	const int circleY = height() - radius_ * 2 - 2;
	const int circleWidth = radius_ * 2 - circlePercent * 2;
	circleRect_ = QRectF(circleX + circlePercent, circleY + circlePercent, circleWidth, circleWidth);

	QPainterPath path;
	path.addRect(barRect_);
	path.addEllipse(circleRect_);
	path.setFillRule(Qt::WindingFill);
	painter->drawPath(path);

	//绘制用户设定值三角号
	if (showUserValue_) {
		if (tickPosition_ == TickPosition_Left || tickPosition_ == TickPosition_Both) {
			QPolygon pts;
			const int offset = 15;
			const double initX = targetX_ - (barWidth_ / 2 + 5);
			const double initY = rulerHeight - (userValue_ - minValue_) * increment;
			pts.append(QPoint(initX, initY));
			pts.append(QPoint(initX - offset, initY - offset / 2));
			pts.append(QPoint(initX - offset, initY + offset / 2));
			painter->setBrush(userValueColor_);
			painter->drawPolygon(pts);
		}

		if (tickPosition_ == TickPosition_Right || tickPosition_ == TickPosition_Both) {
			QPolygon pts;
			const int offset = 15;
			const double initX = targetX_ + (barWidth_ / 2 + 5);
			const double initY = rulerHeight - (userValue_ - minValue_) * increment;
			pts.append(QPoint(initX, initY));
			pts.append(QPoint(initX + offset, initY - offset / 2));
			pts.append(QPoint(initX + offset, initY + offset / 2));
			painter->setBrush(userValueColor_);
			painter->drawPolygon(pts);
		}
	}

	painter->restore();
}

void RulerTemp::drawValue(QPainter* painter)
{
	painter->save();

	QFont font;
	font.setPixelSize(circleRect_.width() * 0.55);
	painter->setFont(font);
	painter->setPen(Qt::white);
	painter->drawText(circleRect_, Qt::AlignCenter, QString("%1").arg(value_));

	painter->restore();
}

double RulerTemp::getMinValue() const
{
	return minValue_;
}

double RulerTemp::getMaxValue() const
{
	return maxValue_;
}

double RulerTemp::getValue() const
{
	return value_;
}

int RulerTemp::getPrecision() const
{
	return precision_;
}

int RulerTemp::getLongStep() const
{
	return longStep_;
}

int RulerTemp::getShortStep() const
{
	return shortStep_;
}

int RulerTemp::getSpace() const
{
	return space_;
}

bool RulerTemp::getShowUserValue() const
{
	return showUserValue_;
}

double RulerTemp::getUserValue() const
{
	return userValue_;
}

QColor RulerTemp::getUserValueColor() const
{
	return userValueColor_;
}

QColor RulerTemp::getBgColorStart() const
{
	return bgColorStart_;
}

QColor RulerTemp::getBgColorEnd() const
{
	return bgColorEnd_;
}

QColor RulerTemp::getLineColor() const
{
	return lineColor_;
}

QColor RulerTemp::getBarBgColor() const
{
	return barBgColor_;
}

QColor RulerTemp::getBarColor() const
{
	return barColor_;
}

RulerTemp::BarPosition RulerTemp::getBarPosition() const
{
	return barPosition_;
}

RulerTemp::TickPosition RulerTemp::getTickPosition() const
{
	return tickPosition_;
}

QSize RulerTemp::sizeHint() const
{
	return QSize{80, 80};
}

QSize RulerTemp::minimumSizeHint() const
{
	return QSize{ 80, 80 };
}

void RulerTemp::setRange(double minValue, double maxValue)
{
	minValue_ = minValue;
	maxValue_ = maxValue;
	update();
}

void RulerTemp::setRange(int minValue, int maxValue)
{
	minValue_ = minValue;
	maxValue_ = maxValue;
	update();
}

void RulerTemp::setMinValue(double minValue)
{
	minValue_ = minValue;
	update();
}

void RulerTemp::setMaxValue(double maxValue)
{
	maxValue_ = maxValue;
	update();
}

void RulerTemp::setValue(double value)
{
	value_ = value;
	update();
}

void RulerTemp::setValue(int value)
{
	value_ = value;
	update();
}

void RulerTemp::setPrecision(int precision)
{
	precision_ = precision;
	update();
}

void RulerTemp::setLongStep(int longStep)
{
	longStep_ = longStep;
	update();
}

void RulerTemp::setShortStep(int shortStep)
{
	shortStep_ = shortStep;
	update();
}

void RulerTemp::setSpace(int space)
{
	space_ = space;
	update();
}

void RulerTemp::setShowUserValue(bool showUserValue)
{
	showUserValue_ = showUserValue;
	update();
}

void RulerTemp::setUserValue(double userValue)
{
	userValue_ = userValue;
	update();
}

void RulerTemp::setUserValue(int userValue)
{
	userValue_ = userValue;
	update();
}

void RulerTemp::setUserValueColor(const QColor& userValueColor)
{
	userValueColor_ = userValueColor;
	update();
}

void RulerTemp::setBgColorStart(const QColor& bgColorStart)
{
	bgColorStart_ = bgColorStart;
	update();
}

void RulerTemp::setBgColorEnd(const QColor& bgColorEnd)
{
	bgColorEnd_ = bgColorEnd;
	update();
}

void RulerTemp::setLineColor(const QColor& lineColor)
{
	lineColor_ = lineColor;
	update();
}

void RulerTemp::setBarBgColor(const QColor& barBgColor)
{
	barBgColor_ = barBgColor;
	update();
}

void RulerTemp::setBarColor(const QColor& barColor)
{
	barColor_ = barColor;
	update();
}

void RulerTemp::setBarPosition(const BarPosition& barPosition)
{
	barPosition_ = barPosition;
	update();
}

void RulerTemp::setTickPosition(const TickPosition& tickPosition)
{
	tickPosition_ = tickPosition;
	update();
}
