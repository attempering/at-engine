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

#ifndef AT_DEF_H__
#define AT_DEF_H__



/* this header needs to be placed before zcom modules
 * so that we can determine whether `real` should be defined or not */
#include "context/at_context.h"



#include "../zcom/zcom.h"

#include "utils/at_utils__def.h"
#include "bias/at_bias__def.h"
#include "mb/at_mb__def.h"
#include "eh/at_eh__def.h"
#include "langevin/at_langevin__def.h"



/* define a long long int type */
#ifndef I32ONLY
typedef long long  llong_t;
#define llong_pfmt "%lld"
#else
typedef long llong_t;
#define llong_pfmt "%ld"
#endif



typedef struct at_t_ {
  double    boltz;            // Boltzmann constant

  double    beta;             // current beta

  double    temp_thermostat;  // thermostat temperature

  double    md_time_step;     // MD integration step for reference
  int       nsttemp;          // interval of tempering, 0: disable, -1: only when doing neighbor searching

  double    energy;           // current coupling energy

  at_bias_t     bias[1];      // high-temperature bias

  at_mb_t       mb[1];        // multiple-bin estimator

  at_langevin_t langevin[1];  // Langevin equation

  at_eh_t       eh[1];        // energy histogram

  zcom_ssm_t    *ssm;         // string allocator

  zcom_mtrng_t  *mtrng;       // random number generator

  char          *rng_file;    // random number file name

  int           nst_log;      // interval of writing trace file, 0: disable, -1: only when doing neighbor searching
  char          *log_file;    // log/trace file name
  zcom_log_t    *log;         // log file

  char          ch_suffix;    // file suffix as a single-digit character
  char          data_dir[32];

  at_utils_t    utils[1];

} at_t;


#endif
