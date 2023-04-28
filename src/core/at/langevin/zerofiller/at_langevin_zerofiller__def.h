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

#ifndef AT_LANGEVIN_ZEROFILLER__DEF_H__
#define AT_LANGEVIN_ZEROFILLER__DEF_H__

typedef struct at_mb_t_ at_mb_t;

/* parameters for the zerofiller */
typedef struct at_langevin_zerofiller_t_ {
  int n; /* same as mb->n */
  double    beta_min; /* same as mb->bmin */
  double    beta_max; /* same as mb->bmax */
  double    beta_del; /* same as mb->bdel */

  double   *w;        /* weights */
  double   *vals;     /* filled values */
  int      *has_vals;

  int      *ib_from_left;
  int      *ib_from_right;

  at_mb_t *mb;
} at_langevin_zerofiller_t;

typedef double (*bin_value_get_func_t)(at_mb_t *mb, int ib, int *has_val, double *w);


#endif
