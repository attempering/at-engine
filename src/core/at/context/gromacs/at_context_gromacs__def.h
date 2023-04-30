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

#include "typedefs.h"
#include "physics.h"
#include "types/simple.h"

typedef gmx_bool at_bool_t;

#define ZCOM__HAVE_REAL 1

#ifndef AT_MPI
  #ifdef GMX_MPI
  #define AT_MPI 1
  #endif
#endif

#endif
