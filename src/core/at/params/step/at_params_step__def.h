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

#ifndef AT_PARAMS_STEP__DEF_H__
#define AT_PARAMS_STEP__DEF_H__

#include "../../context/at_context__def.h"

#ifdef AT__BETA_SCALING_FUNC__
typedef void at_beta_scaling_func_t(
    double beta_before,
    double beta_after,
    double force_scale,
    void *obj);
#endif

typedef struct at_params_step_t_ {
  at_llong_t step;
  at_bool_t is_first_step;
  at_bool_t is_final_step;
  at_bool_t do_trace;
  at_bool_t flush_output;

#ifdef AT__BETA_SCALING_FUNC__
  at_beta_scaling_func_t* beta_scaling_func;
  void* beta_scaling_obj;
#endif

} at_params_step_t;


#endif
