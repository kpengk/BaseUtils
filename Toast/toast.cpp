#include "toast.h"
#include <QLabel>
#include <QTimer>
#include <QBoxLayout>

Toast::Toast(QWidget *parent)
    : QWidget(parent)
    , label_{ new QLabel(this) }
    , timer_{ new QTimer(this) }
{
    setStyleSheet(R"(background-color: rgb(69, 69, 69);
                        font: 10pt "宋体";
                        color: rgb(255, 255, 255);
                        border:2px groove gray;
                        border-radius: 8px;
                        padding: 5px;)");

    QVBoxLayout* layout{ new QVBoxLayout(this) };
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label_);
    setLayout(layout);

    connect(timer_, &QTimer::timeout, this, &Toast::onTimeOut);
    hide();
}

Toast::~Toast()
{
}

void Toast::setText(const QString& str)
{
    label_->setText(str);

    const QFontMetrics fm(QFont{ "宋体", 10 });
    const QRect rec{ fm.boundingRect(str) };
    const auto textWidth{ rec.width() };
    const auto textHeight{ rec.height() };
    setFixedSize(textWidth + 20, textHeight + 20);
}

void Toast::display(int second)
{
    auto widget = qobject_cast<QWidget*>(parent());
    if(widget)
    {
        const int w = widget->size().width();
        const int h = widget->size().height();
    }

    timer_->start(second * 1000);
    show();
}

void Toast::onTimeOut()
{
    timer_->stop();
    hide();
}
