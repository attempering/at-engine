#ifndef AT__CONTEXT_H__
#define AT__CONTEXT_H__


/* This module defines the programming context:
   whether we are programming under the GROMACS environment
   or plain ANSI?
   Specifically, do we have `real` and `gmx_bool`?
*/


#ifdef ADAPTIVE_TEMPERING_FOR_GROMACS

  /* GROMACS context */

  #ifdef HAVE_CONFIG_H
  #include <config.h>
  #endif

  #ifdef GMX_LIB_MPI
  #include <mpi.h>
  #endif
  #ifdef GMX_THREAD_MPI
  #include "tmpi.h"
  #endif

  #include "typedefs.h"
  #include "physics.h"
  #include "types/simple.h"

  typedef gmx_bool at_bool_t;

#else

  /* ANSI context, need to define `at_bool_t` and `real` */

  typedef int at_bool_t;

  #ifndef FALSE
  #define FALSE 0
  #endif

  #ifndef TRUE
  #define TRUE 1
  #endif

#endif /* defined(ADAPTIVE_TEMPERING_FOR_GROMACS) */



#ifndef AT_MPI
  #ifdef GMX_MPI
  #define AT_MPI 1
  #endif
#endif


#endif
