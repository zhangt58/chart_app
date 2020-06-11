#include "mainwindow.h"
#include "fitting.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::set_chart() {
  // Create x,y points for the sine curve.
  QLineSeries *series = new QLineSeries();

  double x = -4.0;
  int n = 100;
  double delt_x = 8.0 / n;
  for (int i = 0; i <= 100; i++, x += delt_x) {
    series->append(x, sin(x));
  }

  // create a chart for sine curve.
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->createDefaultAxes();
  chart->setTitle("Sine Curve with QtCharts");

  ui->chartview->setRenderHint(QPainter::Antialiasing);
  ui->chartview->setChart(chart);
}

void MainWindow::on_updateButton_clicked() {
  QLineSeries *series = new QLineSeries();

  double x = -4.0;
  int n = 100;
  double delt_x = 8.0 / n;
  for (int i = 0; i <= 100; i++, x += delt_x) {
    series->append(x, cos(x));
  }

  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->createDefaultAxes();
  chart->setTitle("Cosine Curve with QtCharts");

  ui->chartview->setChart(chart);
}

int MainWindow::on_fittingButton_clicked() {
  QScatterSeries *series_1 = new QScatterSeries();
  QLineSeries *series_2 = new QLineSeries(); // after fitting

  const int n = 40, m = 3; // 40 measurements, 3 parameters
  double p[m], x[n], opts[LM_OPTS_SZ], info[LM_INFO_SZ];

  /* generate some measurement using the exponential model with
   * parameters (5.0, 0.1, 1.0), corrupted with zero-mean
   * Gaussian noise of s=0.1
   */
  time_t t;
  srand(time(&t));
  for (int i = 0; i < n; ++i) {
    x[i] = (5.0 * exp(-0.1 * i) + 1.0) + gNoise(0.0, 0.1);
    series_1->append(i, x[i]);
  }

  /* initial parameters estimate: (1.0, 0.0, 0.0) */
  p[0] = 1.0;
  p[1] = 0.0;
  p[2] = 0.0;

  /* optimization control parameters; passing to levmar NULL instead of opts
   * reverts to defaults */
  opts[0] = LM_INIT_MU;
  opts[1] = 1E-15;
  opts[2] = 1E-15;
  opts[3] = 1E-20;
  opts[4] = LM_DIFF_DELTA; // relevant only if the finite difference Jacobian
                           // version is used

  /* invoke the optimization function */
  dlevmar_der(expfunc, jacexpfunc, p, x, m, n, 1000, opts, info, NULL, NULL,
              NULL); // with analytic Jacobian
  printf("Levenberg-Marquardt returned in %g iter, reason %g, sumsq %g [%g]\n",
         info[5], info[6], info[1], info[0]);
  printf("Best fit parameters: %.7g %.7g %.7g\n", p[0], p[1], p[2]);

  double xx = 0, dxx = n / 200.0;
  for (int i = 0; i < 200; ++i, xx += dxx) {
    series_2->append(xx, gFit(xx, p));
  }

  /* update chart */
  QChart *chart = new QChart();
  chart->addSeries(series_1);
  chart->addSeries(series_2);
  chart->createDefaultAxes();
  chart->setTitle("Nonlinear Fitting (levmar: Levenberg-Marquardt nonlinear "
                  "regression) MinGW32 with levmar lib");
  ui->chartview->setRenderHint(QPainter::Antialiasing);
  ui->chartview->setChart(chart);
  return 0;
}
