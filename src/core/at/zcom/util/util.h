#ifndef ZCOM__UTIL_H__
#define ZCOM__UTIL_H__


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

#include "../def/def.h"

#ifndef zcom_util__xnew
#define zcom_util__xnew(x, n) \
  if (#n[0] != '1' && (n) <= 0) { \
    fprintf(stderr, "cannot allocate %d objects for %s\n", (int) (n), #x); \
    exit(1); \
  } else if ((x = calloc(n, sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "no memory for %s x %u\n", #x, (unsigned) (n)); \
    exit(1); }
#endif

#ifndef zcom_util__xrenew
#define zcom_util__xrenew(x, n) \
  if ((n) <= 0) { \
    fprintf(stderr, "cannot allocate %d objects for %s\n", (int) (n), #x); \
    exit(1); \
  } else if ((x = realloc(x, (n)*sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "no memory for %s x %u\n", #x, (unsigned) (n)); \
    exit(1); }
#endif


ZCOM__INLINE void zcom_util__perrmsg__(const char *file, int line, const char *why,
    const char *fmt, va_list args);


#ifdef ZCOM__HAVE_VAM

ZCOM__INLINE void zcom_util__perrmsg_(const char *file, int line, const char *why,
    int cond, const char *fmt, ...)
{
  if (cond) {
    va_list args;
    va_start(args, fmt);
    zcom_util__perrmsg__(file, line, why, fmt, args);
    va_end(args);
    exit(1);
  }
}

#define zcom_util__exit_if(cond, fmt, ...) \
  zcom_util__perrmsg_(__FILE__, __LINE__, #cond, cond, fmt, ## __VA_ARGS__)

#define zcom_util__fatal(fmt, ...)  zcom_util__exit_if(1, fmt, ## __VA_ARGS__)

#else /* !ZCOM__HAVE_VAM */

#define ZCOM_UTIL__PERRMSG__(c) {                        \
  if ((#c[0] == '1' && #c[1] == '\0') || c) { \
    va_list args;                             \
    va_start(args, fmt);                      \
    zcom_util__perrmsg__(NULL, -1, NULL, fmt, args);     \
    va_end(args);                             \
    exit(1);                                  \
  } }
ZCOM__INLINE void zcom_util__exit_if(int cond, const char *fmt, ...) PERRMSG__(cond)
ZCOM__INLINE void zcom_util__fatal(const char *fmt, ...) ZCOM_UTIL__PERRMSG__(1)
#undef ZCOM_UTIL__PERRMSG__

#endif /* ZCOM__HAVE_VAM */

#define zcom_util__xfopen(fp, fn, fmt, err) \
  if ((fp = fopen(fn, fmt)) == NULL) { \
    fprintf(stderr, "cannot open file %s\n", fn); err; }


/* swap two variables */
#ifndef zcom_util__xtpswap
#define zcom_util__xtpswap(tp, x, y) { tp dum_; dum_ = (x); (x) = (y); (y) = dum_; }
#endif

#ifndef zcom_util__intswap
#define zcom_util__intswap(x, y) zcom_util__xtpswap(int, x, y)
#endif

#ifndef zcom_util__dblswap
#define zcom_util__dblswap(x, y) zcom_util__xtpswap(double, x, y)
#endif


#define zcom_util__cisalnum(c)   isalnum((unsigned char)(c))
#define zcom_util__cisalpha(c)   isalpha((unsigned char)(c))
#define zcom_util__cisdigit(c)   isdigit((unsigned char)(c))
#define zcom_util__cisxdigit(c)  isxdigit((unsigned char)(c))
#define zcom_util__cisprint(c)   isprint((unsigned char)(c))
#define zcom_util__cisspace(c)   isspace((unsigned char)(c))
#define zcom_util__cislower(c)   islower((unsigned char)(c))
#define zcom_util__cisupper(c)   isupper((unsigned char)(c))
#define zcom_util__ctolower(c)   (char) tolower((unsigned char)(c))
#define zcom_util__ctoupper(c)   (char) toupper((unsigned char)(c))

/* string manipulation */
#define ZSTR_XSPACEL  0x0001
#define ZSTR_XSPACER  0x0002
#define ZSTR_XSPACE   (ZSTR_XSPACEL|ZSTR_XSPACER)
#define ZSTR_COPY     0x0004
#define ZSTR_CAT      0x0008
#define ZSTR_CASE     0x0100
#define ZSTR_UPPER_   0x0200
#define ZSTR_UPPER    (ZSTR_CASE|ZSTR_UPPER_)
#define ZSTR_LOWER    ZSTR_CASE

/* remove leading and trailing spaces */
#define zcom_util__strip(s)  zcom_util__stripx(s, ZSTR_XSPACE)
#define zcom_util__lstrip(s) zcom_util__stripx(s, ZSTR_XSPACEL)
#define zcom_util__rstrip(s) zcom_util__stripx(s, ZSTR_XSPACER)


/* in the follows, size_s means the buffer size of s, i.e., sizeof(s) for static strings */
/* copy the string and convert it to upper/lower case */
#define zcom_util__strcpy2u(s, t, size_s) zcom_util__strcnv(s, t, size_s - 1, ZSTR_COPY|ZSTR_UPPER)
#define zcom_util__strcpy2l(s, t, size_s) zcom_util__strcnv(s, t, size_s - 1, ZSTR_COPY|ZSTR_LOWER)
#define zcom_util__strcpy_sf(s, t, size_s) zcom_util__strcnv(s, t, size_s - 1, ZSTR_COPY)
#define zcom_util__substr(s, t, start, len) zcom_util__strcnv(s, t+start, len, ZSTR_COPY)
/* concatenate strings, the last parameter is the buffer size of s,
 * unlike strncat(), in which it's the number of characters from *t* to be copied.  */
#define zcom_util__strcat_sf(s, t, size_s) zcom_util__strcnv(s, t, size_s - 1, ZSTR_CAT)


#endif
