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
#include <config.h>
#endif

#ifdef GMX_LIB_MPI
#include <mpi.h>
#endif
#ifdef GMX_THREAD_MPI
#include "tmpi.h"
#endif


// We use the following header(s) to check the definition of gmx_bool
//

// In GROMACS 4.6, we only have GMX_VERSION defined in compiling time in
//  $(build)/include/version.h
//
// GMX_VERSION is major*10000 + minor*100 + patch
//
// For example, for GROMACS 4.6.7, major = 4, minor = 6, patch = 7,
// GMX_VERSION = 40607.
//
//
// In GROMACS 2021, we have GMX_VERSION_MAJOR, GMX_VERSION_MINOR and GMX_VERSION_PATCH
// defined as integers, and GMX_VERSION is defined in
//  $(build)/src/gromacs/version.h
//
// GMX_VERSION == 20210007 (major = 2021, minor = 0, patch = 7)
//
//
#if (defined(GMX_VERSION) && (GMX_VERSION < 60000))
// These headers are located in
//  /include (GROMACS 4.6)
//  /src/gromacs/legacyheaders (GROMACS 5.1)
//  nonexistent (GROMACS 2021)
#include "typedefs.h"
#include "types/simple.h"
#else
// This header is introduced in GROMACS 5.1
// (while legacy headers are still supported)
// /src/gromacs/utility (GROMACS 5.1, 2011)
#include "basedefinitions.h"
#endif

typedef gmx_bool at_bool_t;

#define ZCOM__HAVE_REAL 1





// The following headers contains the definition of `BOLTZ`
#if (defined(GMX_VERSION) && (GMX_VERSION < 50100))
// This is header is in
//  /include (GROMACS 4.6)
//  /src/gromacs/legacyheaders (GROMACS 5.0)
//  nonexistent (GROMACS 5.1, 2021)
#include "physics.h"
#else
// nonexistent (GROMACS 4.6, 5.0)
// /src/gromacs/math/units.h (GROMACS 5.1, 2021)
#include "units.h"
#endif



#ifndef AT_MPI
  #ifdef GMX_MPI
  #define AT_MPI 1
  #endif
#endif

#endif
