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

#ifndef AT_MISC__SRC_H__
#define AT_MISC__SRC_H__

#include <unistd.h>

#include "at_misc.h"

#include "params/at_params.h"
#include "utils/at_utils.h"
#include "distr/at_distr.h"
#include "mb/at_mb.h"
#include "driver/at_driver.h"
#include "eh/at_eh.h"



double at__beta_to_temp(const at_t *at, double beta)
{
    return 1.0/(beta * at->sys_params->boltz);
}

double at__temp_to_beta(const at_t *at, double temp)
{
    return 1.0/(temp * at->sys_params->boltz);
}


double at__get_energy(at_t *at)
{
  return at->energy;
}


void at__set_energy(at_t *at, double energy)
{
  at->energy = energy;
}


double at__get_beta(const at_t *at)
{
  return at->beta;
}


void at__set_beta(at_t *at, double beta)
{
  at->beta = beta;
}


double at__get_bias_w_comb(const at_t *at)
{
  return at_distr__get_bias_w_comb(at->distr, at->beta);
}


double at__get_bias_w_deriv(const at_t *at)
{
  return at_distr__get_bias_w_deriv(at->distr, at->beta);
}


#endif
