#ifndef ZCOM__DEF_H__
#define ZCOM__DEF_H__



/* managing storage classes:
   "static" is the safer choice to avoid naming conflicts.

   Example:
   both m.c and n.c include this file,
   m.c --> m.o, n.c --> n.o, m.o+n.o --> a.out
   static is the only way to avoid naming conflict in this case.

   In case that this file is included multiple times,
   ZCOM_XFUNCS should be defined before the first inclusion,
   otherwise it won't be effective in deciding storage classes.
 */
#ifndef ZCOM_STRCLS
  #ifndef ZCOM_XFUNCS
    #define ZCOM_STRCLS static
  #else
    #define ZCOM_STRCLS
  #endif
#endif

/* inline keyword */
#ifndef ZCOM_INLINE
  #if defined(__GNUC__) || defined(__xlC__)
    #define ZCOM_INLINE ZCOM_STRCLS __inline__
  #elif defined(__INTEL_COMPILER)
    #define ZCOM_INLINE ZCOM_STRCLS __inline
  #elif defined(_MSC_VER) || defined(__BORLANDC__)
    #define ZCOM_INLINE __inline ZCOM_STRCLS
  #elif defined(__STDC_VERSION__) && (STDC_VERSION__ >= 199901L)
    #define ZCOM_INLINE ZCOM_STRCLS inline
  #else
    #define ZCOM_INLINE ZCOM_STRCLS
  #endif
#endif

/* restrict keyword */
#ifndef ZCOM_RESTRICT
  #if (defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__xlC__))
    #define ZCOM_RESTRICT __restrict
  #elif defined(__STDC_VERSION__) && (STDC_VERSION__ >= 199901L)
    #define ZCOM_RESTRICT restrict
  #else
    #define ZCOM_RESTRICT
  #endif
#endif

/* macros with variable-length arguments */
#ifndef ZCOM_HAVE_VAM
  #if (  (defined(__GNUC__) && (__GNUC__ >= 3))   \
      || (defined(__xlC__)  && (__xlC__ >= 0x0700)) \
      || (defined(_MSC_VER) && (_MSC_VER >= 1400)) )
    #define ZCOM_HAVE_VAM 1
  #endif
#endif

#ifdef __INTEL_COMPILER
  #pragma warning(disable:981) /* unspecified order warning */
  #pragma warning(disable:177) /* unreferenced function */
  #pragma warning(disable:161) /* unrecognized #pragma, for omp */
#elif defined(__GNUC__) && (__GNUC__ >= 4 && __GNUC_MINOR__ >= 2)
  #pragma GCC diagnostic ignored "-Wunknown-pragmas"
  #pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif

#ifdef _MSC_VER
  #pragma warning(disable:4127) /* conditional expression constant */
  #pragma warning(disable:4505) /* unreferenced function */
  #pragma warning(disable:4514) /* unreferenced inline */
  #pragma warning(disable:4710) /* not inlined */
  #include <stdio.h> /* suppress CRT _s functions warnings */
#endif



#include <float.h>

/* define a real type */
#ifdef HAVE_REAL
  #ifndef ZCOM_HAVE_REAL
  #define ZCOM_HAVE_REAL HAVE_REAL
  #endif
#endif



#ifndef ZCOM_HAVE_REAL
  #define ZCOM_HAVE_REAL 1
  #define real double
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



#endif
