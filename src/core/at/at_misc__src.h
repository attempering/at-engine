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

#include "../zcom/zcom.h"

#include "params/at_params.h"
#include "utils/at_utils.h"
#include "distr/at_distr.h"
#include "mb/at_mb.h"
#include "driver/at_driver.h"
#include "eh/at_eh.h"



double at__beta_to_temp(const at_t *at, double beta) {
    return 1.0/(beta * at->sys_params->boltz);
}

double at__temp_to_beta(const at_t *at, double temp) {
    return 1.0/(temp * at->sys_params->boltz);
}



void at__set_energy(at_t *at, double energy)
{
  at->energy = energy;
}


/* Decide whether to do something such as
 * writing output on a given step
 *
 * By default, the policy is not to do anything on the first step;
 * by do so on the last step
 * If this is not so, set do_on_first_step to AT__TRUE
 */
at_bool_t at__do_on_step(
    const at_params_step_t *step_params,
    at_llong_t nst,
    at_bool_t do_on_first_step)
{
  return (do_on_first_step || !step_params->is_first_step)
     && (step_params->is_last_step
        || (nst > 0 && step_params->step % nst == 0));
}



static void at__write_log(at_t *at,
    const at_params_step_t *step_params,
    int ib, double invw, double t1, double t2,
    double av_energy)
{
  at_utils_log_t *log = at->utils->log;

  zcom_util__exit_if (!log->inited,
      "log is not initialized!\n");

  at_bool_t do_log = at_utils_log__decide_do_log(log, step_params);

  if (do_log) {
    zcom_log_t *zcom_log = log->log;

    if (zcom_log == NULL) {
      at_utils_log__open_file(log);
    }

    if(step_params->flush_output) {
      zcom_log->flags |= ZCOM_LOG__FLUSH_AFTER;
    } else {
      zcom_log->flags ^= ZCOM_LOG__FLUSH_AFTER;
    }

    zcom_log__printf(zcom_log,
        "%10.3f %5d %10.6f %12.3f %12.3f %10.6f %8.3f %8.5f\n",
        step_params->step * at->sys_params->md_time_step, ib, t2 - t1,
        at->energy, av_energy, at->beta, t1, invw);

  }

}



/* write various output files */
void at__output(at_t *at,
          const at_params_step_t *step_params,
          int ib, double invw, double t1, double t2,
          double av_energy)
{
  at__write_log(at, step_params, ib, invw, t1, t2, av_energy);

  if (at__do_on_step(step_params, at->mb->nst_save_av, AT__FALSE)) { /* save averages */
    at_mb__write(at->mb, at->driver->langevin, at->beta);
    at_mb__write_ze_file(at->mb, NULL);
    at_driver_langevin_rng__save(at->driver->langevin->rng);
  }

  if (at->eh->mode != 0) { 
    if (at__do_on_step(step_params, at->eh->nst_save, AT__FALSE)) { /* save energy histograms */
      at_eh__write(at->eh);
      at_eh__reconstruct(at->eh, NULL);
    }
  }

}



#endif
