/* 
 * Copyright (C) 2010-2023  AT-Engine Developers
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

#ifndef AT_CONTEXT_GROMACS__DEF_H__
#define AT_CONTEXT_GROMACS__DEF_H__

/* GROMACS context */

#include "at_context_gromacs_config__def.h"



#if (GMX_VERSION >= 50000)

/* For GROMACS 5.0 or later,
 * the macros GMX_LIB_MPI and GMX_THREAD_MPI
 * are always defined by CMake,
 * but their numerical values may be zeros
 * when the corresponding MPI functionalities
 * are disabled.
 **/

#include "gromacs/utility/gmxmpi.h"

#else

/* For GROMACS 4.6 or earlier,
 * the macros GMX_LIB_MPI and GMX_THREAD_MPI
 * are not defined when the corresponding MPI
 * functionalities are disabled.
 **/

#ifdef GMX_LIB_MPI
#include <mpi.h>
#endif

#ifdef GMX_THREAD_MPI
#include "tmpi.h"
#endif

#endif


/* We use the following header(s) to check the definition of
 * `gmx_bool` and `real`
 **/
#if (GMX_VERSION < 60000)

  /* These headers are located under
   *  /include for GROMACS 4.6
   *  /src/gromacs/legacyheaders for GROMACS 5.0 and 5.1.
   * But they are removed in GROMACS 2021 or later.
   **/
  #include "typedefs.h"
  #include "types/simple.h"

#else

  /* This following header is introduced in GROMACS 2016
   * (while the above legacy headers are still supported)
   * /src/gromacs/utility (GROMACS 2016-2021)
   * /api/legacy/include/gromacs/utility (GROMACS 2022 or later)
   **/

  #include "gromacs/utility/basedefinitions.h"

#endif

typedef gmx_bool at_bool_t;

/* `real` is also defined in "basedefinitions.h"
 * avoid redefining it in zcom.h */
#define ZCOM__HAVE_REAL 1



/* The following headers contains the definition of `BOLTZ`
 *
 * For GROMACS 4.6, it is defined in
 * /include/physics.h
 *
 * For GROMACS 5.0 or 5.1, it is defined in
 * /src/gromacs/legacyheaders/physics.h
 *
 * For GROMACS 2016 or later, it is defined in
 * /src/gromacs/math/units.h
 *
 * To find the location, use the following command
 *
 * find . -name "*.h" | xargs grep "#define BOLTZ "
 */
#if (GMX_VERSION < 50000)
  #include "physics.h"
#elif (GMX_VERSION < 60000)
  #include "gromacs/legacyheaders/physics.h"
#else
  #include "gromacs/math/units.h"
#endif



#ifndef AT_MPI

  #if GMX_VERSION >= 20160000

    /* modern versions, GMX_MPI is always defined as 0 or 1 */

    #if defined(GMX_MPI)
      #define AT_MPI GMX_MPI
    #else
      /* should never reach here */
      #define AT_MPI 0
    #endif

  #else /* legacy versions <= 5.1 */

    #if defined(GMX_MPI)
      #define AT_MPI 1
    #else
      #define AT_MPI 0
    #endif

  #endif

#endif /* defined(AT_MPI) */



#endif
