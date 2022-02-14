/** Toast
 *   @brief Display prompt message window.
 *   @author pengk
 *   @version 1.0
 *   @date   2020.03
 *   @warning
 */

#pragma once
#include <QWidget>

//enum EmPos{UP, DOWN, LEFT, RIGHT, CENTRE};

class QLabel;
class QTimer;
class Toast : public QWidget
{
    Q_OBJECT

public:
    explicit Toast(QWidget *parent = nullptr);
    ~Toast();
    void setText(const QString& str);
    void display(int second = 2);

private slots:
    void onTimeOut();

private:
    QLabel* label_;
    QTimer* timer_;
};
