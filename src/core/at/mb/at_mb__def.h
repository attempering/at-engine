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

/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT_MB_DEF_H__
#define AT_MB_DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sm/at_mb_sm__def.h"
#include "betadist/at_mb_betadist__def.h"
#include "shk/at_mb_shk__def.h"
#include "win/at_mb_win__def.h"
#include "accum/at_mb_accum__def.h"
#include "iie/at_mb_iie__def.h"

typedef struct zcom_ssm_t_ zcom_ssm_t;

/* multiple-bin estimator parameters */
typedef struct at_mb_t_ {
  double    boltz;    /* Boltzmann constant */

  double    bmin;     /* minimal beta (highest temperature) */
  double    bmax;     /* maximal beta (lowest temperature) */
  double    bdel;     /* bin size of beta */
  int       n;        /* number of temperature bins */
  double    *barr;    /* temperature array */
  unsigned  flags;    /* combination of flags */

  int       nstrefresh;   /* interval of recalculating et for all temperature */
  int       nst_save_av;  /* interval of writing mbav and ze files */
  int       av_binary;    /* use binary format in mbav file */
  char      *av_file;     /* name of mbav file */
  char      *ze_file;     /* name of ze file */
  int       wze_reps;     /* number of iterations before writing ze file */
  double    *visits;      /* number of visits */
  double    total_visits; /* total number of visits, number of tempering */

  /* overall beta distribution */
  at_mb_betadist_t betadist[1];

  /* adaptive averaging related parameters */
  at_mb_shk_t  shk[1];

  /* window settings */
  at_mb_win_t win[1];

  /* accumulators */
  at_mb_accum_t accum[1];

  at_mb_iie_t  iie[1];

  zcom_ssm_t     *ssm;     /* string allocator */

  int       cnt_int;  /* number of additional integer variables to be written to binary file */
  int       cnt_dbl;  /* number of additional double variables to be written to binary file */

} at_mb_t;

#define   MB_USE_WIN_ACCUM       0x00000001    /* use adaptive averaging */
#define   MB_CV         0x00000002    /* compute heat capacity */
#define   MB_SYMWIN     0x00000004    /* use symmetrical windows */
#define   MB_SINGLE_BIN 0x00000020    /* use the single-bin estimator */
#define   MB_VERBOSE    0x00001000    /* be verbose */



#endif


