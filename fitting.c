////////////////////////////////////////////////////////////////////////////////////
//  Example program that shows how to use levmar in order to fit the three-
//  parameter exponential model x_i = p[0]*exp(-p[1]*i) + p[2] to a set of
//  data measurements; example is based on a similar one from GSL.
//
//  Copyright (C) 2008  Manolis Lourakis (lourakis at ics forth gr)
//  Institute of Computer Science, Foundation for Research & Technology - Hellas
//  Heraklion, Crete, Greece.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
////////////////////////////////////////////////////////////////////////////////////

#include "fitting.h"

/* Gaussian noise with mean m and variance s, uses the Box-Muller transformation
 */
double gNoise(double m, double s) {
  double r1, r2, val;

  r1 = ((double)rand()) / DBL_RAND_MAX;
  r2 = ((double)rand()) / DBL_RAND_MAX;

  val = sqrt(-2.0 * log(r1)) * cos(2.0 * M_PI * r2);

  val = s * val + m;

  return val;
}

double gFit(double x, double *p) { return p[0] * exp(-p[1] * x) + p[2]; }

/* model to be fitted to measurements: x_i = p[0]*exp(-p[1]*i) + p[2], i=0...n-1
 */
void expfunc(double *p, double *x, int m, int n, void *data) {
  register int i;

  for (i = 0; i < n; ++i) {
    x[i] = p[0] * exp(-p[1] * i) + p[2];
  }
}

/* Jacobian of expfunc() */
void jacexpfunc(double *p, double *jac, int m, int n, void *data) {
  register int i, j;

  /* fill Jacobian row by row */
  for (i = j = 0; i < n; ++i) {
    jac[j++] = exp(-p[1] * i);
    jac[j++] = -p[0] * i * exp(-p[1] * i);
    jac[j++] = 1.0;
  }
}
