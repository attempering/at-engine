/* 
 * Copyright (C) 2006-2023  AT-Engine Developers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef ZCOM__UTILS__SRC_H__
#define ZCOM__UTILS__SRC_H__


#include "utils.h"

/* print an error message */
void zcom_utils__perrmsg__(const char *file, int line, const char *reason,
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


ZCOM__INLINE int zcom_utils__fexists(const char *fn)
{
  FILE *fp;
  if ((fp = fopen(fn, "r")) == NULL) return 0;
  else { fclose(fp); return 1; }
}


ZCOM__INLINE int zcom_utils__intmax(int x, int y) { return x > y ? x : y; }
ZCOM__INLINE int zcom_utils__intmin(int x, int y) { return x < y ? x : y; }
/* confine x within [xmin, xmax] */
ZCOM__INLINE int zcom_utils__int_confined(int x, int xmin, int xmax)
  { return x < xmin ? xmin : x > xmax ? xmax : x; }

ZCOM__INLINE int zcom_utils__intsqr(int x) { return x * x; }

/* get the pair index from 0 to n*(n - 1)/2 - 1 */
ZCOM__INLINE int zcom_utils__get_pair_index(int i, int j, int n)
{
  zcom_utils__exit_if (i < 0 || i >= n || j < 0 || j >= n || i == j,
      "bad index error i %d, j %d, n %d\n", i, j, n);
  if (i > j) { int i1 = i; i = j; j = i1; }
  return n*i - (i + 1)*(i + 2)/2 + j;
}

/* return individual indices for a given pair index */
ZCOM__INLINE int zcom_utils__parse_pair_index(int id, int n, int *i, int *j)
{
  int i1, n1;
  zcom_utils__exit_if (id < 0 || id >= n*(n - 1)/2, "index %d too large for n %d\n", id, n);
  for (i1 = n - 2; i1 >= 0; i1--) {
    if (id >= (n1 = i1*n - i1*(i1 + 1)/2)) {
      *i = i1;
      *j = id - n1 + i1 + 1;
      return 0;
    }
  }
  return 1;
}

ZCOM__INLINE double zcom_utils__dblmax(double x, double y) { return x > y ? x : y; }
ZCOM__INLINE double zcom_utils__dblmin(double x, double y) { return x < y ? x : y; }
/* confine x within [xmin, xmax] */
ZCOM__INLINE double zcom_utils__dbl_confined(double x, double xmin, double xmax)
  { return x < xmin ? xmin : x > xmax ? xmax : x; }

ZCOM__INLINE double zcom_utils__dblsqr(double x) { return x * x; }

/* sqrt(x*x + y*y) */
ZCOM__INLINE double zcom_utils__dblhypot(double x, double y)
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
ZCOM__INLINE double zcom_utils__dblround(double x, double dx)
{
  if (x*dx > 0) return dx * (int)(x/dx + (.5 - DBL_EPSILON));
  else return -dx * (int)(-x/dx + (.5 - DBL_EPSILON));
}

ZCOM__INLINE void zcom_utils__dblcleararr(double *x, int n)
  { int i; for (i = 0; i < n; i++) x[i] = 0.0; }




ZCOM__INLINE char *zcom_utils__strip_(char *s, unsigned flags)
{
  char *p;

  if (flags & zcom_utils__REMOVE_LEADING_SPACES) { /* remove leading spaces */
    for (p = s; zcom_utils__isspace(*p); p++) ;
    if (*p == '\0') {
      *s = '\0';
    }
    else {
      memmove(s, p, strlen(p)+1);
    }
  }

  if (flags & zcom_utils__REMOVE_TRAILING_SPACES) { /* remove trailing spaces */
    for (p = s + strlen(s) - 1; p >= s && zcom_utils__isspace(*p); p--) {
      *p = '\0';
    }
  }

  return s;
}



ZCOM__INLINE char *zcom_utils__str_to_lower(char *s)
{
  char *p;

  for (p = s; *p != '\0'; p++) {
    *p = zcom_utils__tolower(*p);
  }

  return s;
}



ZCOM__INLINE char *zcom_utils__str_to_upper(char *s)
{
  char *p;

  for (p = s; *p != '\0'; p++) {
    *p = zcom_utils__toupper(*p);
  }

  return s;
}



/* safely copy/cat strings with case conversion
 * unlike strncpy(), s is always null-terminated on return: it copies at most
 * len nonblank characters, i.e., s[len] = '\0' for the longest output */
ZCOM__INLINE char *zcom_utils__strcnv(char *s, const char *t, size_t len, unsigned flags)
{
  size_t i = 0, j;
  unsigned docase = flags & zcom_utils__STR_CASE, up = flags & zcom_utils__STR_UPPER_;

  if (len == 0 || s == NULL || t == NULL) return s;
  if (flags & zcom_utils__STR_CONCAT) while(s[i]) i++;
  for (j = 0; i < len; i++, j++) {
    if (docase && t[j]) {
      if (up) s[i] = (char) (unsigned char) toupper((unsigned char) t[j]);
      else    s[i] = (char) (unsigned char) tolower((unsigned char) t[j]);
    } else s[i] = t[j];
    if (t[j] == 0) break;
  }
  if (i == len) s[i] = '\0';
  if (flags & zcom_utils__REMOVE_SPACES) zcom_utils__strip_(s, flags); /* call zcom_utils__strip */
  return s;
}

/* compare strings without case */
#define strcmpnc(s, t) zcom_utils__strncmpnc(s, t, -1)
ZCOM__INLINE int zcom_utils__strncmpnc(const char *s, const char *t, int n)
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
