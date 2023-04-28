/* 
 * Copyright (C) 2010-2023  At-engine Developers
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

#ifndef AT_CONTEXT_H__
#define AT_CONTEXT_H__


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
