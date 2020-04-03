#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCharts/QLineSeries>
#include <QPainter>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::set_chart()
{
    QLineSeries *series = new QLineSeries();

    double x = -4.0;
    int n = 100;
    double delt_x = 8.0 / n;
    for(int i=0; i<=100; i++, x+=delt_x) {
        series->append(x, sin(x));
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Sine Curve with QtChart");

    ui->chartview->setRenderHint(QPainter::Antialiasing);
    ui->chartview->setChart(chart);
}
