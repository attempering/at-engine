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

#ifndef AT_EH_RECON__DEF_H__
#define AT_EH_RECON__DEF_H__

typedef struct at_eh_recon_t_
{
  int        n;               /* number of temperature bins */

  double     e_min;           /* minimal energy */
  double     e_max;           /* maximal energy */
  double     e_del;           /* energy bin size */
  int        e_n;             /* number of energy bins */

  int        bwmod;           /* 0: d(beta) 1: dT/T  2: d(kT) */
  double     bwdel;           /* delta lnT */
  int        stride;    /* interval of reconstructing energy histograms */
  char      *file;      /* name of reconstructed energy histogram */
  double    *recon;     /* temporary space for reconstructing histogram */
  int       *is;        /* indices for temperature windows (lower) */
  int       *it;        /* indices for temperature windows (higher) */

} at_eh_recon_t;

#endif
