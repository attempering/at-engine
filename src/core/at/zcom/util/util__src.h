#ifndef ZCOM__UTIL__SRC_H__
#define ZCOM__UTIL__SRC_H__


#include "util.h"


/* print an error message */
ZCOM__INLINE void zcom_util__perrmsg__(const char *file, int line, const char *reason,
    const char *fmt, va_list args)
{
  fprintf(stderr, "error: ");
  vfprintf(stderr, fmt, args);
  if (fmt[strlen(fmt) - 1] != '\n')
    fprintf(stderr, "\n"); /* add a new line if needed */
  
  if (file != NULL && line >= 0) {
    /* this format help VS code to locate the position */
    fprintf(stderr, "file: %s:%d\n", file, line);
  } else {
    if (file != NULL) fprintf(stderr, "file: %s\n", file);
    if (line >= 0) fprintf(stderr, "line: %d\n", line);
  }

  if (reason != NULL && strcmp(reason, "1") != 0) {
    fprintf(stderr, "cond: %s\n", reason);
  }

}


ZCOM__INLINE int zcom_util__fexists(const char *fn)
{
  FILE *fp;
  if ((fp = fopen(fn, "r")) == NULL) return 0;
  else { fclose(fp); return 1; }
}


ZCOM__INLINE int zcom_util__intmax(int x, int y) { return x > y ? x : y; }
ZCOM__INLINE int zcom_util__intmin(int x, int y) { return x < y ? x : y; }
/* confine x within [xmin, xmax] */
ZCOM__INLINE int zcom_util__int_confined(int x, int xmin, int xmax)
  { return x < xmin ? xmin : x > xmax ? xmax : x; }

ZCOM__INLINE int zcom_util__intsqr(int x) { return x * x; }

/* get the pair index from 0 to n*(n - 1)/2 - 1 */
ZCOM__INLINE int zcom_util__get_pair_index(int i, int j, int n)
{
  zcom_util__exit_if (i < 0 || i >= n || j < 0 || j >= n || i == j,
      "bad index error i %d, j %d, n %d\n", i, j, n);
  if (i > j) { int i1 = i; i = j; j = i1; }
  return n*i - (i + 1)*(i + 2)/2 + j;
}

/* return individual indices for a given pair index */
ZCOM__INLINE int zcom_util__parse_pair_index(int id, int n, int *i, int *j)
{
  int i1, n1;
  zcom_util__exit_if (id < 0 || id >= n*(n - 1)/2, "index %d too large for n %d\n", id, n);
  for (i1 = n - 2; i1 >= 0; i1--) {
    if (id >= (n1 = i1*n - i1*(i1 + 1)/2)) {
      *i = i1;
      *j = id - n1 + i1 + 1;
      return 0;
    }
  }
  return 1;
}

ZCOM__INLINE double zcom_util__dblmax(double x, double y) { return x > y ? x : y; }
ZCOM__INLINE double zcom_util__dblmin(double x, double y) { return x < y ? x : y; }
/* confine x within [xmin, xmax] */
ZCOM__INLINE double zcom_util__dbl_confined(double x, double xmin, double xmax)
  { return x < xmin ? xmin : x > xmax ? xmax : x; }

ZCOM__INLINE double zcom_util__dblsqr(double x) { return x * x; }

/* sqrt(x*x + y*y) */
ZCOM__INLINE double zcom_util__dblhypot(double x, double y)
{
  double t;
  x = fabs(x);
  y = fabs(y);
  if (x <= 0.) return y;
  else if (y <= 0.) return x;
  if (x < y) t = x, x = y, y = t;
  t = y/x;
  return x*sqrt(1+t*t);
}

/* round x to a multiple dx  */
ZCOM__INLINE double zcom_util__dblround(double x, double dx)
{
  if (x*dx > 0) return dx * (int)(x/dx + (.5 - DBL_EPSILON));
  else return -dx * (int)(-x/dx + (.5 - DBL_EPSILON));
}

ZCOM__INLINE void zcom_util__dblcleararr(double *x, int n)
  { int i; for (i = 0; i < n; i++) x[i] = 0.0; }

#ifndef LNADD_DEFINED
#define LNADD_DEFINED
#define LN_BIG 50.0

/* log(exp(a) + exp(b)) */
ZCOM__INLINE double zcom_util__lnadd(double a, double b)
{
  double c;
  if (a < b) { c = a; a = b; b = c; } /* ensure a >= b */
  return ((c = a-b) > LN_BIG) ? a : a + log(1 + exp(-c));
}

/* log(exp(a) - exp(b)), only works for a > b */
ZCOM__INLINE double zcom_util__lndif(double a, double b)
{
  double c;
  zcom_util__exit_if (a < b, "zcom_util__lndif: %g < %g\n", a, b);
  return ((c = a-b) > LN_BIG) ? a : a + log(1 - exp(-c));
}

/* log(exp(a)+b) */
ZCOM__INLINE double zcom_util__lnaddn(double a, double b)
{
  return (a > LN_BIG) ? a : a + log(1 + b*exp(-a));
}

#undef LN_BIG
#endif  /* LNADD_DEFINED */


ZCOM__INLINE char *zcom_util__stripx(char *s, unsigned flags)
{
  char *p;

  if (flags & ZSTR_XSPACEL) { /* remove leading spaces */
    for (p = s; zcom_util__cisspace(*p); p++) ;
    if (*p == '\0') *s = '\0';
    else memmove(s, p, strlen(p)+1);
  }
  if (flags & ZSTR_XSPACER) /* remove trailing spaces */
    for (p = s + strlen(s) - 1; p >= s && zcom_util__cisspace(*p); p--)
      *p = '\0';
  return s;
}

/* safely copy/cat strings with case conversion
 * unlike strncpy(), s is always null-terminated on return: it copies at most
 * len nonblank characters, i.e., s[len] = '\0' for the longest output */
ZCOM__INLINE char *zcom_util__strcnv(char *s, const char *t, size_t len, unsigned flags)
{
  size_t i = 0, j;
  unsigned docase = flags & ZSTR_CASE, up = flags & ZSTR_UPPER_;

  if (len == 0 || s == NULL || t == NULL) return s;
  if (flags & ZSTR_CAT) while(s[i]) i++;
  for (j = 0; i < len; i++, j++) {
    if (docase && t[j]) {
      if (up) s[i] = (char) (unsigned char) toupper((unsigned char) t[j]);
      else    s[i] = (char) (unsigned char) tolower((unsigned char) t[j]);
    } else s[i] = t[j];
    if (t[j] == 0) break;
  }
  if (i == len) s[i] = '\0';
  if (flags & ZSTR_XSPACE) zcom_util__stripx(s, flags); /* call zcom_util__strip */
  return s;
}

/* compare strings without case */
#define strcmpnc(s, t) zcom_util__strncmpnc(s, t, -1)
ZCOM__INLINE int zcom_util__strncmpnc(const char *s, const char *t, int n)
{
  int i, cs, ct;

  if (s == NULL || t == NULL) return 0;
  for (i = 0; ; i++) {
    if (i >= n) return 0;
    cs = s[i];
    ct = t[i];
    if (cs == 0 || ct == 0) break;
    cs = toupper( (unsigned char) cs );
    ct = toupper( (unsigned char) ct );
    if (cs != ct) break;
  }
  return cs-ct;
}

#endif