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

#ifndef ATGMX_CONTEXT_H__
#define ATGMX_CONTEXT_H__


#ifndef GMX_VERSION
#include "gromacs/version.h"
#endif


#include <string.h>
#include <signal.h>
#include <omp.h>

// /src/gromacs/utility (5.1, 2021)
// /include (4.6)
#include "gromacs/utility/smalloc.h"

// /src/gromacs/gmxlib (2021)
#include "gromacs/gmxlib/network.h"

// /src/gromacs/mdlib (2021)
#include "gromacs/mdlib/vsite.h"

#include "gromacs/mdlib/enerdata_utils.h"
#include "gromacs/mdtypes/enerdata.h"
#include "gromacs/mdtypes/pull_params.h"
#include "gromacs/mdtypes/multipletimestepping.h"
#include "gromacs/utility/keyvaluetree.h"

// /src/gromacs/mdtypes/inputrec.h (2021)
// t_inputrec
#include "gromacs/mdtypes/inputrec.h"

// /src/gromacs/mdtypes/commrec.h (2021)
// t_commrec, MASTER, PAR
#include "gromacs/mdtypes/commrec.h"

// /src/gromacs/mdtypes/enerdata.h (2021)
// gmx_enerdata_t
#include "gromacs/mdtypes/enerdata.h"

// /src/gromacs/mdtypes/mdatom.h (2021)
// t_mdatoms
#include "gromacs/mdtypes/mdatom.h"

// /src/gromacs/mdtypes/forcebuffers.h (2021)
// gmx::ForceBuffersView
#include "gromacs/mdtypes/forcebuffers.h"

// gmx_omp_nthreads_get();
#include "gromacs/mdlib/gmx_omp_nthreads.h"

// getThreadAtomRange();
#include "gromacs/mdlib/update.h"

// /src/gromacs/commandline/filenm.h (2021)
// t_filenm
#include "gromacs/commandline/filenm.h"

// /src/gromacs/mdrunutility/multisim.h (2021)
// gmx_multisim_t
#include "gromacs/mdrunutility/multisim.h"

// mpi.h or tmpi.h
#include "gromacs/utility/gmxmpi.h"

#endif
