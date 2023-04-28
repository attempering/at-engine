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

#ifndef AT_BIAS__DEF_H__
#define AT_BIAS__DEF_H__

/* high-temperature bias

  H = kappa* H0 + epsilon * H1
  kappa = 1-(T-Tref)*(1-kappa0)/(Tmax-Tref) if T>Tref; kappa=1 if T<Tref
  epsilon= epsilon0*(T-Tref)/(Tmax-Tref) if T>Tref; epsilon=0 if T<Tref

*/
typedef struct at_bias_t_
{
  int       enabled;
  double    TH_Tref;
  double    *kappa, *epsilon;
  double    kappa0, epsilon0;
} at_bias_t;

#endif
