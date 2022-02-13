/** Toast
 *   @brief Display prompt message window.
 *   @author pengk
 *   @version 1.0
 *   @date   2020.03
 *   @warning
 */

#ifndef TOAST_H
#define TOAST_H

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
    void setText(QString str);
    void display(int second = 2);

private slots:
    void onTimeOut();

private:
    QLabel* m_label;
    QTimer* m_timer;
};

#endif // TOAST_H
