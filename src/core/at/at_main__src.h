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

#ifndef AT_MAIN__SRC_H__
#define AT_MAIN__SRC_H__


#include "at_main.h"

#include "../zcom/zcom.h"
#include "utils/at_utils.h"



/* return a pointer of an initialized at_t
 * if possible, initial values are taken from configuration
 * file `cfg`, otherwise default values are assumed */
static at_t *at__cfg_open(const char *cfg_filename, double boltz, double md_time_step, int isuffix)
{
  zcom_cfg_t *cfg;
  at_t *at;
  at_bool_t bLoaded;

  /* open configuration file */
  zcom_util__exit_if(!(cfg = zcom_cfg__open(cfg_filename)),
      "at_t: cannot open configuration file %s.\n", cfg_filename);

  /* allocate memory for at_t */
  zcom_util__exit_if ((at = (at_t *) calloc(1, sizeof(at_t))) == NULL,
      "Fatal: no memory for a new object of at_t\n");

  at->boltz = boltz;

  /* call low level function */
  zcom_util__exit_if (!(bLoaded = at__cfg_init(at, cfg, isuffix, boltz, md_time_step)),
    "at_t: error while reading configuration file %s\n", cfg_filename);

  fprintf(stderr, "Successfully loaded configuration file %s\n", cfg_filename);

  at_langevin_rng__reset(at->langevin->rng, 0);

  /* close handle to configuration file */
  zcom_cfg__close(cfg);

  return at;
}


at_t *at__open(
    const char *zcom_cfg_fn,
    at_bool_t bCPT,
    at_bool_t open_log,
    double boltz,
    double time_step,
    int suffix)
{
  at_t *at;

  /* this will also initialize settings for member objects such as at->mb */
  at = at__cfg_open((zcom_cfg_fn != NULL) ? zcom_cfg_fn : "at.cfg", boltz, time_step, suffix);
  zcom_util__exit_if(at == NULL, "failed to load configuration file.\n");

  fprintf(stderr, "initial temperature set to %g, beta %g\n", at->temp_thermostat, at__beta_to_temp(at->temp_thermostat, at->boltz));

  /* make the initial temperature = temp_thermostat */
  at->beta = at__temp_to_beta(at->temp_thermostat, at->boltz);

  /* we only load previous data if it's continuation */
  if (at__load_data(at, bCPT) != 0) {
    fprintf(stderr, "Warning: This simulation is started from checkpoint, while some files are missing. Will assume no previous simulation data is available.\n");
  }

  if (open_log) {
    at_utils_log__open_file(at->utils->log);
  }

  return at;
}



int at__move(at_t *at, llong_t step, at_bool_t bfirst, at_bool_t blast, at_bool_t btr, at_bool_t bflush)
{
  double invwf = 1.0, temp1, temp2, Eav = 0.0, neg_dlnwf_dbeta;
  int ib, rep;

  temp1 = at__beta_to_temp(at->beta, at->boltz);

  ib = at_mb__beta_to_index(at->mb, at->beta, 1);

  /* update energy data, change at->beta */
  /* repeat several times to change the temperature */
  for (rep = 0; rep < at->langevin->move_repeats; rep++) {
    /* 1. deposit the current energy and temperature */
    at_mb__add(at->mb, at->energy, at->beta, &ib, &invwf, &neg_dlnwf_dbeta);

    /* 2. add the current data point to the energy histogram */
    at_eh__add(at->eh, ib, at->energy);

    /* 3. use the Langevin equation to update the temperature */
    at->beta = at_langevin__move(at->langevin, at->mb, at->energy, at->beta,
        ib, invwf, neg_dlnwf_dbeta, &Eav);
  }

  temp2 = at__beta_to_temp(at->beta, at->boltz);

  if (at__do_every(step, at->mb->nstrefresh, 0, blast)) {
    at_mb__refresh_et(at->mb, 1);
  }

  at__output(at, step, ib, invwf, temp1, temp2, Eav, bfirst, blast, btr, bflush);

  return 0;
}


#endif
