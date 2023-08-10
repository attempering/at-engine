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

#ifndef AT_CONTEXT_GROMACS_CONFIG__DEF_H__
#define AT_CONTEXT_GROMACS_CONFIG__DEF_H__

/* GROMACS context */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifndef GMX_DOUBLE
#define GMX_DOUBLE 0
#endif

#ifndef GMX_GPU_CUDA
#define GMX_GPU_CUDA 0
#endif

#ifndef GMX_GPU_OPENCL
#define GMX_GPU_OPENCL 0
#endif

#ifndef GMX_GPU_SYCL
#define GMX_GPU_SYCL 0
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


/* define GMX_LIB_MPI, GMX_THREAD_MPI and GMX_MPI */
#if GMX_VERSION >= 20160000

#ifndef GMX_LIB_MPI
#define GMX_LIB_MPI 0
#endif

#ifndef GMX_THREAD_MPI
#define GMX_THREAD_MPI 0
#endif

#ifndef GMX_MPI
#define GMX_MPI (GMX_LIB_MPI || GMX_THREAD_MPI)
#endif

#endif /* GMX_VERSION >= 20160000 */



#endif
