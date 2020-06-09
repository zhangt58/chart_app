#ifndef FITTING_H
#define FITTING_H

#include <gsl/gsl_blas.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>

#ifdef __cplusplus
extern "C" {
struct data {
  double *t;
  double *y;
  size_t n;
};
double gaussian(const double, const double, const double, const double);
int func_f(const gsl_vector *, void *, gsl_vector *);
int func_df(const gsl_vector *, void *, gsl_matrix *);
int func_fvv(const gsl_vector *, const gsl_vector *, void *, gsl_vector *fvv);
void solve_system(gsl_vector *, gsl_multifit_nlinear_fdf *,
                  gsl_multifit_nlinear_parameters *);
}
#endif //__cplusplus

#endif // FITTING_H
