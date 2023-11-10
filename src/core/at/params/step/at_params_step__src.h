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

#ifndef AT_PARAMS_STEP__SRC_H__
#define AT_PARAMS_STEP__SRC_H__

#include "at_params_step.h"

void at_params_step__init(at_params_step_t* params)
{
  params->step = 0;
  params->is_first_step = AT__FALSE;
  params->is_final_step = AT__FALSE;
  params->do_trace = AT__FALSE;
  params->flush_output = AT__FALSE;

#ifdef AT__BETA_SCALING_FUNC__
  params->beta_scaling_func = NULL;
  params->beta_scaling_obj = NULL;
#endif

}

#endif
