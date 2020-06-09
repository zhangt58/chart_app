#include "fitting.h"

struct data {
  double *t;
  double *y;
  size_t n;
};

/* model function: a * exp( -1/2 * [ (t - b) / c ]^2 ) */
double gaussian(const double a, const double b, const double c,
                const double t) {
  const double z = (t - b) / c;
  return (a * exp(-0.5 * z * z));
}

int func_f(const gsl_vector *x, void *params, gsl_vector *f) {
  struct data *d = (struct data *)params;
  double a = gsl_vector_get(x, 0);
  double b = gsl_vector_get(x, 1);
  double c = gsl_vector_get(x, 2);
  size_t i;

  for (i = 0; i < d->n; ++i) {
    double ti = d->t[i];
    double yi = d->y[i];
    double y = gaussian(a, b, c, ti);

    gsl_vector_set(f, i, yi - y);
  }

  return GSL_SUCCESS;
}

int func_df(const gsl_vector *x, void *params, gsl_matrix *J) {
  struct data *d = (struct data *)params;
  double a = gsl_vector_get(x, 0);
  double b = gsl_vector_get(x, 1);
  double c = gsl_vector_get(x, 2);
  size_t i;

  for (i = 0; i < d->n; ++i) {
    double ti = d->t[i];
    double zi = (ti - b) / c;
    double ei = exp(-0.5 * zi * zi);

    gsl_matrix_set(J, i, 0, -ei);
    gsl_matrix_set(J, i, 1, -(a / c) * ei * zi);
    gsl_matrix_set(J, i, 2, -(a / c) * ei * zi * zi);
  }

  return GSL_SUCCESS;
}

int func_fvv(const gsl_vector *x, const gsl_vector *v, void *params,
             gsl_vector *fvv) {
  struct data *d = (struct data *)params;
  double a = gsl_vector_get(x, 0);
  double b = gsl_vector_get(x, 1);
  double c = gsl_vector_get(x, 2);
  double va = gsl_vector_get(v, 0);
  double vb = gsl_vector_get(v, 1);
  double vc = gsl_vector_get(v, 2);
  size_t i;

  for (i = 0; i < d->n; ++i) {
    double ti = d->t[i];
    double zi = (ti - b) / c;
    double ei = exp(-0.5 * zi * zi);
    double Dab = -zi * ei / c;
    double Dac = -zi * zi * ei / c;
    double Dbb = a * ei / (c * c) * (1.0 - zi * zi);
    double Dbc = a * zi * ei / (c * c) * (2.0 - zi * zi);
    double Dcc = a * zi * zi * ei / (c * c) * (3.0 - zi * zi);
    double sum;

    sum = 2.0 * va * vb * Dab + 2.0 * va * vc * Dac + vb * vb * Dbb +
          2.0 * vb * vc * Dbc + vc * vc * Dcc;

    gsl_vector_set(fvv, i, sum);
  }

  return GSL_SUCCESS;
}

void callback(const size_t iter, void *params,
              const gsl_multifit_nlinear_workspace *w) {
  gsl_vector *f = gsl_multifit_nlinear_residual(w);
  gsl_vector *x = gsl_multifit_nlinear_position(w);
  double avratio = gsl_multifit_nlinear_avratio(w);
  double rcond;

  (void)params; /* not used */

  /* compute reciprocal condition number of J(x) */
  gsl_multifit_nlinear_rcond(&rcond, w);

  fprintf(stderr,
          "iter %2zu: a = %.4f, b = %.4f, c = %.4f, |a|/|v| = %.4f cond(J) = "
          "%8.4f, |f(x)| = %.4f\n",
          iter, gsl_vector_get(x, 0), gsl_vector_get(x, 1),
          gsl_vector_get(x, 2), avratio, 1.0 / rcond, gsl_blas_dnrm2(f));
}

void solve_system(gsl_vector *x, gsl_multifit_nlinear_fdf *fdf,
                  gsl_multifit_nlinear_parameters *params) {
  const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
  const size_t max_iter = 200;
  const double xtol = 1.0e-8;
  const double gtol = 1.0e-8;
  const double ftol = 1.0e-8;
  const size_t n = fdf->n;
  const size_t p = fdf->p;
  gsl_multifit_nlinear_workspace *work =
      gsl_multifit_nlinear_alloc(T, params, n, p);
  gsl_vector *f = gsl_multifit_nlinear_residual(work);
  gsl_vector *y = gsl_multifit_nlinear_position(work);
  int info;
  double chisq0, chisq, rcond;

  /* initialize solver */
  gsl_multifit_nlinear_init(x, fdf, work);

  /* store initial cost */
  gsl_blas_ddot(f, f, &chisq0);

  /* iterate until convergence */
  gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol, callback, NULL, &info,
                              work);

  /* store final cost */
  gsl_blas_ddot(f, f, &chisq);

  /* store cond(J(x)) */
  gsl_multifit_nlinear_rcond(&rcond, work);

  gsl_vector_memcpy(x, y);

  /* print summary */

  fprintf(stderr, "NITER         = %zu\n", gsl_multifit_nlinear_niter(work));
  fprintf(stderr, "NFEV          = %zu\n", fdf->nevalf);
  fprintf(stderr, "NJEV          = %zu\n", fdf->nevaldf);
  fprintf(stderr, "NAEV          = %zu\n", fdf->nevalfvv);
  fprintf(stderr, "initial cost  = %.12e\n", chisq0);
  fprintf(stderr, "final cost    = %.12e\n", chisq);
  fprintf(stderr, "final x       = (%.12e, %.12e, %12e)\n",
          gsl_vector_get(x, 0), gsl_vector_get(x, 1), gsl_vector_get(x, 2));
  fprintf(stderr, "final cond(J) = %.12e\n", 1.0 / rcond);

  gsl_multifit_nlinear_free(work);
}
