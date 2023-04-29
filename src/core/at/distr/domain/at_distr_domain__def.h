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

#ifndef AT_DISTR_DOMAIN__DEF_H__
#define AT_DISTR_DOMAIN__DEF_H__


typedef struct at_distr_domain_t_
{
  double    boltz;    /* Boltzmann constant */

  double    bmin;     /* minimal beta (highest temperature) */
  double    bmax;     /* maximal beta (lowest temperature) */
  double    bdel;     /* bin size of beta */
  int       n;        /* number of temperature bins */
  double    *barr;    /* temperature array */
} at_distr_domain_t;


#endif
