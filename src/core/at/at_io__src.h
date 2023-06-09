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

#ifndef AT_IO__SRC_H___
#define AT_IO__SRC_H___

#include <unistd.h>

#include "at_io.h"

#include "../zcom/zcom.h"

#include "params/at_params.h"
#include "utils/at_utils.h"
#include "distr/at_distr.h"
#include "mb/at_mb.h"
#include "driver/at_driver.h"
#include "eh/at_eh.h"



/* Decide whether to do something such as
 * writing output on a given step
 *
 * By default, the policy is not to do anything on the first step;
 * by do so on the last step
 * If this is not so, set do_on_first_step to AT__TRUE
 */
at_bool_t at__do_output_on_step(
    const at_params_step_t *step_params,
    at_llong_t nst,
    at_bool_t do_on_first_step)
{
  return (do_on_first_step || !step_params->is_first_step)
     && (step_params->is_last_step
        || (nst > 0 && step_params->step % nst == 0));
}



static void at__write_trace(at_t *at,
    const at_params_step_t *step_params,
    int ib, double invw, double t1, double t2,
    double av_energy)
{
  at_utils_trace_t *trace = at->utils->trace;

  zcom_util__exit_if (!trace->inited,
      "log is not initialized!\n");

  at_bool_t do_trace = at_utils_trace__decide_do_trace(trace, step_params);

  if (do_trace) {
    zcom_log_t *zcom_log = trace->log;

    if (zcom_log == NULL) {
      zcom_log = at_utils_trace__open_file(trace, 0);
    }

    //fprintf(stderr, "step params %p, zcom_log %p\n", step_params, zcom_log);

    if(step_params->flush_output) {
      zcom_log->flags |= ZCOM_LOG__FLUSH_AFTER;
    } else {
      zcom_log->flags ^= ZCOM_LOG__FLUSH_AFTER;
    }

    zcom_log__printf(zcom_log,
        "%10.3f %5d %10.6f %12.3f %12.3f %10.6f %8.3f %8.5f %5.3f %g %g\n",
        step_params->step * at->sys_params->md_time_step,
        ib,
        t2 - t1,
        at->energy,
        av_energy,
        at->beta,
        t1,
        invw,
        at_driver_langevin_move__get_acceptance_ratio(at->driver->langevin),
        at->driver->langevin->total,
        at->driver->langevin->n_exemption);

  }

}



/* write various output files */
void at__output(at_t *at,
          const at_params_step_t *step_params)
{
  // to save mb and langevin data
  if (at__do_output_on_step(step_params, at->mb->nst_save_av, AT__FALSE)) {
    at_mb__write(at->mb);
    at_driver__write(at->driver);
    //at_mb__write_legacy(at->mb, at->driver->langevin, at->beta);
    at_mb__write_ze_file(at->mb, NULL);
    at_driver_langevin_rng__save(at->driver->langevin->rng);
  }

  // write suggestions
  if (at__do_output_on_step(step_params, at->driver->langevin->nst_suggest, AT__FALSE)) {
    at_driver_langevin_move__print_acceptance_ratio_suggestions(at->driver->langevin);
  }

  // to save energy histogram data
  if (at->eh->mode != 0) { 
    if (at__do_output_on_step(step_params, at->eh->nst_save, AT__FALSE)) {
      at_eh__write(at->eh);
      at_eh__reconstruct(at->eh, NULL);
    }
  }

}



/* load previous data */
int at__load_data(at_t *at, at_bool_t is_continuation)
{
  at_mb_t *mb = at->mb;
  int load_data;

  if (!is_continuation) { /* initial run */
    return 0;
  }

  load_data = is_continuation;
  if (load_data) {
    /* read previous at_mb_t data */
    /*
    if (at_mb__read_legacy(mb, at->driver->langevin, &at->beta) != 0) {
      fprintf(stderr, "cannot load mb data from %s\n", mb->av_file);
      return 1;
    }
    */

    if (at_mb__read(mb) != 0) {
      fprintf(stderr, "cannot load mb data from %s\n", mb->av_file);
      return 1;
    }

    at_mb__write_ze_file(mb, mb->ze_init_file);



    if (at_driver__read(at->driver) != 0) {
      fprintf(stderr, "cannot load driver data from %s\n", at->driver->langevin->file);
      return 1;
    }

    /* read previous energy histogram data */
    if (at_eh__read(at->eh) != 0) {
      fprintf(stderr, "cannot load energy histogram from %s\n", at->eh->file);
      return 2;
    }
  }

  fprintf(stderr, "successfully load previous data\n");

  return 0;
}



#endif
