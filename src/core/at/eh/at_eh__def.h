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

#ifndef AT_EH__DEF_H__
#define AT_EH__DEF_H__

#include "../mb/at_mb__def.h"

typedef struct at_eh_t_
{
  int        n;         /* number of temperature bins */
  int        mode;      /* 0: disable; 1: simple histogram */
  int        recon_stride;      /* interval of reconstructing energy histograms */
  int        bwmod;     /* 0: d(beta) 1: dT/T  2: d(kT) */
  double     bwdel;     /* delta lnT */
  double     min;       /* minimal energy */
  double     max;       /* maximal energy */
  double     del;       /* energy bin size */
  int        cnt;       /* number of energy bins */
  int        binary;    /* use binary format for IO */
  int        nst_save;  /* interval of writing histogram files */
  char      *file;      /* name of IO file */
  char      *rfile;     /* name of reconstructed energy histogram */
  double    *his;       /* energy histogram data */
  double    *recon;     /* temporary space for reconstructing histogram */
  int       *is;        /* indices for temperature windows (lower) */
  int       *it;        /* indices for temperature windows (higher) */
  unsigned   flags;
  at_mb_t    *mb;       /* associated at_mb_t object */

  at_bool_t keep_margins;
  at_bool_t add_half_ebin;
  at_bool_t no_zeros;

  double min_real;
  double max_real;

} at_eh_t;

/* Legacy IO flags */
#define   AT_EH__ADD_HALF_EBIN  0x00010000    /* add a half energy bin width in output */
#define   AT_EH__KEEP_MARGINS   0x00020000    /* keep zero edge at sides */
#define   AT_EH__NO_ZEROS       0x00040000    /* do not output zeroes */

#endif
