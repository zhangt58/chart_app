#include "mainwindow.h"
#include "fitting.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QtCharts/QLineSeries>
#include <cmath>
#include <cstdlib>
#include <cstdio>

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
  const size_t n = 300;  /* number of data points to fit */
  const size_t p = 3;    /* number of model parameters */
  const double a = 5.0;  /* amplitude */
  const double b = 0.4;  /* center */
  const double c = 0.15; /* width */
  const gsl_rng_type *T = gsl_rng_default;
  gsl_vector *f = gsl_vector_alloc(n);
  gsl_vector *x = gsl_vector_alloc(p);
  gsl_multifit_nlinear_fdf fdf;
  gsl_multifit_nlinear_parameters fdf_params =
      gsl_multifit_nlinear_default_parameters();
  struct data fit_data;
  gsl_rng *r;
  size_t i;

  gsl_rng_env_setup();
  r = gsl_rng_alloc(T);

  fit_data.t = (double *) malloc(n * sizeof(double));
  fit_data.y = (double *) malloc(n * sizeof(double));
  fit_data.n = n;

  /* generate synthetic data with noise */
  for (i = 0; i < n; ++i) {
    double t = (double)i / (double)n;
    double y0 = gaussian(a, b, c, t);
    double dy = gsl_ran_gaussian(r, 0.1 * y0);

    fit_data.t[i] = t;
    fit_data.y[i] = y0 + dy;
  }

  /* define function to be minimized */
  fdf.f = func_f;
  fdf.df = func_df;
  fdf.fvv = func_fvv;
  fdf.n = n;
  fdf.p = p;
  fdf.params = &fit_data;

  /* starting point */
  gsl_vector_set(x, 0, 1.0);
  gsl_vector_set(x, 1, 0.0);
  gsl_vector_set(x, 2, 1.0);

  fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;
  solve_system(x, &fdf, &fdf_params);

  /* print data and model */
  QLineSeries *series_1 = new QLineSeries();
  QLineSeries *series_2 = new QLineSeries(); // after fitting
  {
    double A = gsl_vector_get(x, 0);
    double B = gsl_vector_get(x, 1);
    double C = gsl_vector_get(x, 2);

    for (i = 0; i < n; ++i) {
      double ti = fit_data.t[i];
      double yi = fit_data.y[i];
      double fi = gaussian(A, B, C, ti);

      series_1->append(ti, yi);
      series_2->append(ti, fi);
      printf("%f %f %f\n", ti, yi, fi);
    }
  }

  gsl_vector_free(f);
  gsl_vector_free(x);
  gsl_rng_free(r);

  /* update chart */
  QChart *chart = new QChart();
  chart->addSeries(series_1);
  chart->addSeries(series_2);
  chart->createDefaultAxes();
  chart->setTitle("Nonlinear Fitting (GSL: Levenberg-Marquardt with geodesic acceleration)");

  ui->chartview->setRenderHint(QPainter::Antialiasing);
  ui->chartview->setChart(chart);

  return 0;
}
