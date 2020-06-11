#ifndef FITTING_H
#define FITTING_H

#include <levmar.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef LM_DBL_PREC
#error Example program assumes that levmar has been compiled with double precision, see LM_DBL_PREC!
#endif

/* the following macros concern the initialization of a random number generator
 * for adding noise */
#undef REPEATABLE_RANDOM
#define DBL_RAND_MAX (double)(RAND_MAX)

#ifdef _MSC_VER // MSVC
#include <process.h>
#define GETPID _getpid
#elif defined(__GNUC__) // GCC
#include <sys/types.h>
#include <unistd.h>
#define GETPID getpid
#else
#warning Do not know the name of the function returning the process id for your OS/compiler combination
#define GETPID 0
#endif /* _MSC_VER */

#ifdef REPEATABLE_RANDOM
#define INIT_RANDOM(seed) srandom(seed)
#else
#define INIT_RANDOM(seed) srandom((int)GETPID()) // seed unused
#endif

#ifdef __cplusplus
extern "C" {
double gNoise(double, double);
double gFit(double, double *);
void expfunc(double *, double *, int, int, void *);
void jacexpfunc(double *, double *, int, int, void *);
}
#endif //__cplusplus

#endif // FITTING_H
