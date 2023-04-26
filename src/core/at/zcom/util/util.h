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

#ifndef xnew
#define xnew(x, n) \
  if (#n[0] != '1' && (n) <= 0) { \
    fprintf(stderr, "cannot allocate %d objects for %s\n", (int) (n), #x); \
    exit(1); \
  } else if ((x = calloc(n, sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "no memory for %s x %u\n", #x, (unsigned) (n)); \
    exit(1); }
#endif

#ifndef xrenew
#define xrenew(x, n) \
  if ((n) <= 0) { \
    fprintf(stderr, "cannot allocate %d objects for %s\n", (int) (n), #x); \
    exit(1); \
  } else if ((x = realloc(x, (n)*sizeof(*(x)))) == NULL) { \
    fprintf(stderr, "no memory for %s x %u\n", #x, (unsigned) (n)); \
    exit(1); }
#endif


ZCOM_INLINE void perrmsg__(const char *file, int line, const char *why,
    const char *fmt, va_list args);


#ifdef ZCOM_HAVE_VAM

ZCOM_INLINE void perrmsg_(const char *file, int line, const char *why,
    int cond, const char *fmt, ...)
{
  if (cond) {
    va_list args;
    va_start(args, fmt);
    perrmsg__(file, line, why, fmt, args);
    va_end(args);
    exit(1);
  }
}

#define exit_if(cond, fmt, ...) \
  perrmsg_(__FILE__, __LINE__, #cond, cond, fmt, ## __VA_ARGS__)

#define fatal(fmt, ...)  exit_if(1, fmt, ## __VA_ARGS__)

#else /* !ZCOM_HAVE_VAM */

#define PERRMSG__(c) {                        \
  if ((#c[0] == '1' && #c[1] == '\0') || c) { \
    va_list args;                             \
    va_start(args, fmt);                      \
    perrmsg__(NULL, -1, NULL, fmt, args);     \
    va_end(args);                             \
    exit(1);                                  \
  } }
ZCOM_INLINE void exit_if(int cond, const char *fmt, ...) PERRMSG__(cond)
ZCOM_INLINE void fatal(const char *fmt, ...) PERRMSG__(1)
#undef PERRMSG__

#endif /* ZCOM_HAVE_VAM */

#define xfopen(fp, fn, fmt, err) \
  if ((fp = fopen(fn, fmt)) == NULL) { \
    fprintf(stderr, "cannot open file %s\n", fn); err; }


/* swap two variables */
#ifndef xtpswap
#define xtpswap(tp, x, y) { tp dum_; dum_ = (x); (x) = (y); (y) = dum_; }
#endif

#ifndef intswap
#define intswap(x, y) xtpswap(int, x, y)
#endif

#ifndef dblswap
#define dblswap(x, y) xtpswap(double, x, y)
#endif


#define cisalnum(c)   isalnum((unsigned char)(c))
#define cisalpha(c)   isalpha((unsigned char)(c))
#define cisdigit(c)   isdigit((unsigned char)(c))
#define cisxdigit(c)  isxdigit((unsigned char)(c))
#define cisprint(c)   isprint((unsigned char)(c))
#define cisspace(c)   isspace((unsigned char)(c))
#define cislower(c)   islower((unsigned char)(c))
#define cisupper(c)   isupper((unsigned char)(c))
#define ctolower(c)   (char) tolower((unsigned char)(c))
#define ctoupper(c)   (char) toupper((unsigned char)(c))

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
#define strip(s)  stripx(s, ZSTR_XSPACE)
#define lstrip(s) stripx(s, ZSTR_XSPACEL)
#define rstrip(s) stripx(s, ZSTR_XSPACER)


/* in the follows, size_s means the buffer size of s, i.e., sizeof(s) for static strings */
/* copy the string and convert it to upper/lower case */
#define strcpy2u(s, t, size_s) strcnv(s, t, size_s - 1, ZSTR_COPY|ZSTR_UPPER)
#define strcpy2l(s, t, size_s) strcnv(s, t, size_s - 1, ZSTR_COPY|ZSTR_LOWER)
#define strcpy_sf(s, t, size_s) strcnv(s, t, size_s - 1, ZSTR_COPY)
#define substr(s, t, start, len) strcnv(s, t+start, len, ZSTR_COPY)
/* concatenate strings, the last parameter is the buffer size of s,
 * unlike strncat(), in which it's the number of characters from *t* to be copied.  */
#define strcat_sf(s, t, size_s) strcnv(s, t, size_s - 1, ZSTR_CAT)


#endif
