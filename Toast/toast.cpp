#include "toast.h"
#include <QLabel>
#include <QTimer>
#include <QBoxLayout>

Toast::Toast(QWidget *parent) : QWidget(parent)
{
    this->setStyleSheet(R"(background-color: rgb(69, 69, 69);
                        font: 10pt "宋体";
                        color: rgb(255, 255, 255);
                        border:2px groove gray;
                        border-radius: 8px;
                        padding: 5px;)");
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    m_label = new QLabel(this);
    layout->addWidget(m_label);
    this->setLayout(layout);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Toast::onTimeOut);
    this->hide();
}

Toast::~Toast()
{
}

void Toast::setText(QString str)
{
    m_label->setText(str);
    QFont font;
    font.setFamily("宋体");
    font.setPointSize(10);
    QFontMetrics fm(font);
    QRect rec = fm.boundingRect(str);
    //字符串所占的像素宽度,高度
    int textWidth = rec.width();
    int textHeight = rec.height();
    this->setFixedSize(textWidth + 20, textHeight + 20);
}

void Toast::display(int second)
{
    /*QWidget* widget = dynamic_cast<QWidget*>(this->parent());
    if(widget)
    {
        const int w = widget->size().width();
        const int h = widget->size().height();
    }*/
    m_timer->start(second * 1000);
    this->show();
}

void Toast::onTimeOut()
{
    m_timer->stop();
    this->hide();
}
