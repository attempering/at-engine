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

#ifndef ZCOM__UTILS_H__
#define ZCOM__UTILS_H__


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#include "../def/def.h"

#ifndef zcom_utils__new
#define zcom_utils__new(x, n) \
  if (#n[0] != '1' && (n) <= 0) { \
    fprintf(stderr, "Error@zcom.utils: failed to allocate %d objects for %s\n", (int) (n), #x); \
    exit(1); \
  } else if ((x = calloc(n, sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "Error@zcom.utils: no memory for %s x %u\n", #x, (unsigned) (n)); \
    exit(1); }
#endif

#ifndef zcom_utils__renew
#define zcom_utils__renew(x, n) \
  if ((n) <= 0) { \
    fprintf(stderr, "Error@zcom.utils: failed to allocate %d objects for %s\n", (int) (n), #x); \
    exit(1); \
  } else if ((x = realloc(x, (n)*sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "Error@zcom.utils: no memory for %s x %u\n", #x, (unsigned) (n)); \
    exit(1); }
#endif


void zcom_utils__perrmsg__(const char *file, int line, const char *reason,
    const char *fmt, va_list args);


#ifdef ZCOM__HAVE_VAM

ZCOM__INLINE void zcom_utils__perrmsg_(const char *file, int line, const char *why,
    int cond, const char *fmt, ...)
{
  if (cond) {
    va_list args;
    va_start(args, fmt);
    zcom_utils__perrmsg__(file, line, why, fmt, args);
    va_end(args);
    exit(1);
  }
}

#define zcom_utils__exit_if(cond, fmt, ...) \
  zcom_utils__perrmsg_(__FILE__, __LINE__, #cond, cond, fmt, ## __VA_ARGS__)

#define zcom_utils__fatal(fmt, ...)  zcom_utils__exit_if(1, fmt, ## __VA_ARGS__)

#else /* !ZCOM__HAVE_VAM */

#define zcom_utils__PERRMSG__(c) {                        \
  if ((#c[0] == '1' && #c[1] == '\0') || c) { \
    va_list args;                             \
    va_start(args, fmt);                      \
    zcom_utils__perrmsg__(NULL, -1, NULL, fmt, args);     \
    va_end(args);                             \
    exit(1);                                  \
  } }
void zcom_utils__exit_if(int cond, const char *fmt, ...) PERRMSG__(cond)
void zcom_utils__fatal(const char *fmt, ...) zcom_utils__PERRMSG__(1)
#undef zcom_utils__PERRMSG__

#endif /* ZCOM__HAVE_VAM */

#define zcom_utils__xfopen(fp, fn, fmt, err) \
  if ((fp = fopen(fn, fmt)) == NULL) { \
    fprintf(stderr, "Error@zcom.utils: failed to open file %s\n", fn); err; }


/* swap two variables */
#ifndef zcom_utils__xtpswap
#define zcom_utils__xtpswap(tp, x, y) { tp dum_; dum_ = (x); (x) = (y); (y) = dum_; }
#endif

#ifndef zcom_utils__intswap
#define zcom_utils__intswap(x, y) zcom_utils__xtpswap(int, x, y)
#endif

#ifndef zcom_utils__dblswap
#define zcom_utils__dblswap(x, y) zcom_utils__xtpswap(double, x, y)
#endif


#define zcom_utils__isalnum(c)   isalnum((unsigned char)(c))
#define zcom_utils__isalpha(c)   isalpha((unsigned char)(c))
#define zcom_utils__isdigit(c)   isdigit((unsigned char)(c))
#define zcom_utils__isxdigit(c)  isxdigit((unsigned char)(c))
#define zcom_utils__isprint(c)   isprint((unsigned char)(c))
#define zcom_utils__isspace(c)   isspace((unsigned char)(c))
#define zcom_utils__islower(c)   islower((unsigned char)(c))
#define zcom_utils__isupper(c)   isupper((unsigned char)(c))
#define zcom_utils__tolower(c)   (char) tolower((unsigned char)(c))
#define zcom_utils__toupper(c)   (char) toupper((unsigned char)(c))

/* string manipulation */
#define zcom_utils__REMOVE_LEADING_SPACES   0x0001
#define zcom_utils__REMOVE_TRAILING_SPACES  0x0002
#define zcom_utils__REMOVE_SPACES   (zcom_utils__REMOVE_LEADING_SPACES|zcom_utils__REMOVE_TRAILING_SPACES)
#define zcom_utils__STR_COPY     0x0004
#define zcom_utils__STR_CONCAT   0x0008
#define zcom_utils__STR_CASE     0x0100
#define zcom_utils__STR_UPPER_   0x0200
#define zcom_utils__STR_UPPER    (zcom_utils__STR_CASE|zcom_utils__STR_UPPER_)
#define zcom_utils__STR_LOWER    zcom_utils__STR_CASE

/* remove leading and trailing spaces */
char *zcom_utils__strip_(char *s, unsigned flags);

#define zcom_utils__strip(s)  zcom_utils__strip_(s, zcom_utils__REMOVE_SPACES)
#define zcom_utils__lstrip(s) zcom_utils__strip_(s, zcom_utils__REMOVE_LEADING_SPACES)
#define zcom_utils__rstrip(s) zcom_utils__strip_(s, zcom_utils__REMOVE_TRAILING_SPACES)


char *zcom_utils__str_to_lower(char *s);
char *zcom_utils__str_to_upper(char *s);


/* in the following macros,
   size_s means the buffer size of s, i.e., sizeof(s) for static strings */
/* copy the string and convert it to upper/lower case */
#define zcom_utils__strcpy2u(s, t, size_s) zcom_utils__strcnv(s, t, size_s - 1, zcom_utils__STR_COPY|zcom_utils__STR_UPPER)
#define zcom_utils__strcpy2l(s, t, size_s) zcom_utils__strcnv(s, t, size_s - 1, zcom_utils__STR_COPY|zcom_utils__STR_LOWER)
#define zcom_utils__strcpy_sf(s, t, size_s) zcom_utils__strcnv(s, t, size_s - 1, zcom_utils__STR_COPY)
#define zcom_utils__substr(s, t, start, len) zcom_utils__strcnv(s, t+start, len, zcom_utils__STR_COPY)

/* concatenate strings, the last parameter is the buffer size of s,
 * unlike strncat(), in which it's the number of characters from *t* to be copied.  */
#define zcom_utils__strcat_sf(s, t, size_s) zcom_utils__strcnv(s, t, size_s - 1, zcom_utils__STR_CONCAT)


#endif
