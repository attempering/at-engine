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
  int        n;               /* number of temperature bins */
  int        mode;            /* 0: disable; 1: simple histogram */

  double     e_min;           /* minimal energy */
  double     e_max;           /* maximal energy */
  double     e_del;           /* energy bin size */
  int        e_n;             /* number of energy bins */

  int        nst_save;        /* interval of writing histogram files */
  at_bool_t  use_text_file;   /* use text format for data storage */
  char      *file_binary;     /* name of the binary file */
  char      *file_text;       /* name of the text file */

  double    *his;       /* energy histogram data */

  int        bwmod;           /* 0: d(beta) 1: dT/T  2: d(kT) */
  double     bwdel;           /* delta lnT */
  int        recon_stride;    /* interval of reconstructing energy histograms */
  char      *file_recon;      /* name of reconstructed energy histogram */
  double    *recon;     /* temporary space for reconstructing histogram */
  int       *is;        /* indices for temperature windows (lower) */
  int       *it;        /* indices for temperature windows (higher) */

  unsigned   flags;
  at_mb_t    *mb;       /* reference handle */

  at_bool_t keep_margins;
  at_bool_t add_half_ebin;
  at_bool_t no_zeros;

  double e_min_runtime;
  double e_max_runtime;

} at_eh_t;

/* Legacy IO flags */
#define   AT_EH__ADD_HALF_EBIN  0x00010000    /* add a half energy bin width in output */
#define   AT_EH__KEEP_MARGINS   0x00020000    /* keep zero edge at sides */
#define   AT_EH__NO_ZEROS       0x00040000    /* do not output zeroes */

#endif
