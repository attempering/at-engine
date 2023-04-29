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

#ifndef AT_BASIC__SRC_H__
#define AT_BASIC__SRC_H__

#include <unistd.h>

#include "at_basic.h"

#include "../zcom/zcom.h"

#include "distr/at_distr.h"
#include "mb/at_mb.h"
#include "langevin/at_langevin.h"
#include "eh/at_eh.h"


/* load previous data */
int at__load_data(at_t *at, at_bool_t bCPT)
{
  at_mb_t *mb = at->mb;
  int read_mb = 1;

  if (!bCPT) { /* initial run */
    return 0;
  }

  /*read_mb = (at->premode > 0) ? 0 : bCPT;*/
  read_mb = bCPT;
  if (read_mb) {
     /* read previous at_mb_t data */
    if (at_mb__read(mb, at->langevin, &at->beta) != 0) {
      fprintf(stderr, "cannot load mb data from %s\n", mb->av_file);
      return 1;
    }

    at_mb__write_ze_file(mb, "ze_init.dat");

    /* read previous energy histogram data */
    if (at_eh__read(at->eh) != 0) {
      fprintf(stderr, "cannot load energy histogram from %s\n", at->eh->file);
      return 2;
    }
  }

  fprintf(stderr, "successfully load previous data\n");

  return 0;
}



/* don't write at the first step; write at the last step */
int at__do_every(at_llong_t step, int nst, at_bool_t bfirst, at_bool_t blast)
{
  return !bfirst && (blast || (nst > 0 && step % nst == 0));
}



/* write various output files */
void at__output(at_t *at, at_llong_t step,
          int ib, double invw, double t1, double t2, double Eav,
          at_bool_t is_first_step, at_bool_t is_last_step,
          at_bool_t do_log, at_bool_t flush_output)
{
  if (at->utils->log->inited) {

    zcom_log_t *log = at->utils->log->log;

    if (log != NULL) {

      int nst_log = at->utils->log->nst_log;

      // whether to write the log file
      if (nst_log > 0) {
        do_log = (step % nst_log == 0) || is_first_step || is_last_step;
      } else if (nst_log < 0) {
        // logging is disabled if at->nst_log < 0
        do_log = 0;
      }

      if (do_log) {

        if(flush_output) {
          log->flags |= ZCOM_LOG__FLUSH_AFTER;
        } else {
          log->flags ^= ZCOM_LOG__FLUSH_AFTER;
        }

        zcom_log__printf(log,
            "%10.3f %5d %10.6f %12.3f %12.3f %10.6f %8.3f %8.5f",
            step * at->md_time_step, ib, t2 - t1,
            at->energy, Eav, at->beta, t1, invw);
        zcom_log__printf(log, "\n");
      }
    }
  }

  if (at__do_every(step, at->mb->nst_save_av, is_first_step, is_last_step)) { /* save averages */
    at_mb__write(at->mb, at->langevin, at->beta);
    at_mb__write_ze_file(at->mb, NULL);
    at_langevin_rng__save(at->langevin->rng);
  }

  if (at->eh->mode != 0) { 
    if (at__do_every(step, at->eh->nst_save, is_first_step, is_last_step)) { /* save energy histograms */
      at_eh__write(at->eh);
      at_eh__reconstruct(at->eh, NULL);
    }
  }

}



int at__cfg_init(at_t *at, zcom_cfg_t *cfg, int isuffix, double boltz, double md_time_step)
{
  /* temp_thermostat: thermostat temperature */
  at->temp_thermostat = 300.0;
  if (zcom_cfg__get(cfg, &at->temp_thermostat, "T0", "%lf")) {
    fprintf(stderr, "assuming default: at->temp_thermostat = 300.0, key: T0\n");
  }

  /* nsttemp: frequency of tempering, 0: disable, -1: only ns */
  at->nsttemp = -1;
  if (zcom_cfg__get(cfg, &at->nsttemp, "nsttemp", "%d")) {
    fprintf(stderr, "assuming default: at->nsttemp = -1, key: nsttemp\n");
  }

  /* md_time_step: MD integration step, for convenience */
  at->md_time_step = md_time_step;

  int silent = 0;
  const char *data_dir;
  zcom_ssm_t *ssm;

  at_utils__cfg_init(at->utils, cfg, isuffix, silent);

  data_dir = at->utils->data_dir;
  ssm = at->utils->ssm;

  at_distr__cfg_init(at->distr, cfg, silent);

  /* handler for multiple-bin estimator */
  at_mb__cfg_init(at->mb, cfg, boltz, ssm, data_dir, silent);

  at_langevin__cfg_init(at->langevin, at->mb, cfg, ssm, data_dir, silent);

  /* energy histogram */
  at_eh__cfg_init(at->eh, at->mb, cfg, ssm, data_dir, silent);

  at->energy = 0.0;

  return 1;
}


void at__finish(at_t *at)
{
  at_utils__finish(at->utils);

  at_eh__finish(at->eh);

  at_mb__finish(at->mb);

  at_langevin__finish(at->langevin);

  at_distr__finish(at->distr);

  memset(at, 0, sizeof(*at));
}


void at__close(at_t *at)
{
  at__finish(at);
  free(at);
}


int at__manifest(at_t *at)
{
  at_utils_manifest_t *manifest = at->utils->manifest;
  FILE *fp = at_utils_manifest__open_file(manifest);

  fprintf(fp, "at->temp_thermostat: double, %g\n", at->temp_thermostat);
  fprintf(fp, "at->nsttemp: int, %4d\n", at->nsttemp);
  fprintf(fp, "at->md_time_step: double, %g\n", at->md_time_step);

  at_utils__manifest(at->utils);

  at_distr__manifest(at->distr, manifest);

  if (at->mb != NULL) {
    fprintf(fp, "at->mb: object pointer to at_mb_t\n");
    at_mb__manifest(at->mb, manifest);
  }

  if (at->eh->mode != 0) {
    at_eh__manifest(at->eh, manifest);
  }

  if (at->langevin != NULL) {
    fprintf(fp, "at->langevin: object pointer to at_langevin_t\n");
    at_langevin__manifest(at->langevin, manifest);
  }

  fprintf(fp, "at->energy: double, %g\n", at->energy);

  at_utils_manifest__close_file(manifest);

  return 0;
}



#endif
