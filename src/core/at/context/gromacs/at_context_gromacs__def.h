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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


/*
 * Make sure GMX_VERSION is defined 
 *
 * For GROMACS 5.0 or later,
 * GMX_VERSION is defined in src/gromacs/version.h
 * under the build directory.
 *
 * For GROMACS 4.6
 * GMX_VERSION is defined in `include/version.h`
 * under the build directory `include`, and it is then
 * included in `include/sysstuff.h` under
 * the source directory `include`.
 * The latter is included in many other headers,
 * such as "typedefs.h"
 * Hopefully, one of these headers has been included
 * before this one, then GMX_VERSION is defined.
 *
 * If this is not so, we may resort to "config.h"
 * which defines two macros
 * that are removed in later versions of GROMACS,
 * which are `VERSION` and `USE_GROMACS_H`
 *
 **/
#ifndef GMX_VERSION
  #if (defined(VERSION) && defined(USE_VERSION_H))
    #define GMX_VERSION 40608
  #else
    #include "gromacs/version.h"
  #endif
#endif


/* GMX_VERSION should be defined now */
#ifndef GMX_VERSION
#error "GMX_VERSION is not defined"
#endif

/* Numerical GROMACS version, GMX_VERSION
 *
 * In GROMACS 4.6, we only have GMX_VERSION defined in compiling time in
 *  $(build)/include/version.h
 *
 * GMX_VERSION is major*10000 + minor*100 + patch
 *
 * For example, for GROMACS 4.6.7, major = 4, minor = 6, patch = 7,
 * GMX_VERSION = 40607.
 *
 *
 * In GROMACS 2021, we have GMX_VERSION_MAJOR, GMX_VERSION_MINOR and GMX_VERSION_PATCH
 * defined as integers, and GMX_VERSION is defined in
 *  $(build)/src/gromacs/version.h
 *
 * GMX_VERSION == 20210007 (major = 2021, minor = 0, patch = 7)
 * 
 **/



/*
#ifdef GMX_VERSION
#error "GMX_VERSION is defined"
#if GMX_VERSION < 50000
#error "GROMACS 4 or earlier"
#endif
#else
#error "$GMX_VERSION is not defined"
#endif
*/


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
  #ifdef GMX_MPI
  #define AT_MPI 1
  #endif
#endif



#endif
