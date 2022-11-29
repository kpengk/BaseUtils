#include <QApplication>
#include <QMainWindow>
#include <QLineSeries>
#include <numeric>
#include <QtMath>
#include "ChartView.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto series{ new QLineSeries() };

    for (int i = -8 * M_PI * 5.0; i < 8 * M_PI * 5.0; ++i) {
        const auto val = i / 5.0;
        series->append(val, std::sin(val) * val);
    }

    auto chart{ new QChart() };
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("zoom and move");

    auto chartView{ new ChartView(chart) };
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(400, 300);
    window.show();

    return a.exec();
}
