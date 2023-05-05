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

#ifndef ATGMX_UTILS__SRC_H__
#define ATGMX_UTILS__SRC_H__

#include "atgmx.h"

#include "at-engine/at-engine.h"



/* to be used as a replacement of opt2fn(),
 * it will replace the file extension from .mdp to .cfg */
char *atgmx__opt2fn(const char *opt, int nfile, const t_filenm fnm[])
{
  int i;

  for (i = 0; i < nfile; i++) {

    if (strcmp(opt, fnm[i].opt) == 0) { /* a match is found */
      char *fname = fnm[i].fns[0], *p;

      if (fnm[i].ftp == efCFG) {
        /* modify the extension from .mdp to .cfg */
        if (strcmp(fname, "grompp.mdp") == 0) { /* replace the default name by NULL */
          return NULL; /* we do not have default name for .cfg files */
        } else if ((p = strstr(fname, ".cfg.mdp")) != NULL) {
          p[4] = '\0';
        } else if ((p = strstr(fname, ".mdp")) != NULL) {
          strcpy(p, ".cfg");
        }
      }

      return fname;
    }

  }

  return NULL;
}



/* update the current scale for all nodes */
static void atgmx__update_force_scale(atgmx_t *atgmx, t_commrec *cr)
{
  if (!atgmx->enabled) {
    return;
  }

  /* call the single processor version */
  if (MASTER(cr)) {
    at__update_force_scale(atgmx->at);
  }

#ifdef GMX_MPI
  /* inform all other PP nodes the new scale(s) */
  if (PAR(cr)) {
    gmx_bcast(sizeof(atgmx->at->force_scale), &atgmx->at->force_scale, cr);
  }
#endif

}



at_bool_t atgmx__do_tempering(atgmx_t *atgmx, at_llong_t step,
    at_bool_t is_ns_step, at_bool_t is_last_step)
{
  if (!atgmx->enabled) {
    return AT__FALSE;
  }

  int nsttemp = atgmx->at->driver->nsttemp;
  at_bool_t do_tempering = (nsttemp > 0) || is_ns_step || is_last_step;

  if (nsttemp > 0) {
    if ((step % nsttemp) != 0 && !is_last_step) {
      do_tempering = AT__FALSE; /* if nsttemp is set, do tempering at a regular interval */
    }
  }

  return do_tempering;
}



static void atgmx__sum_energy(
    atgmx_t *atgmx,
    real *eterm,
    t_commrec *cr,
    at_llong_t step,
    at_bool_t dirty)
{
  double epot = eterm[F_EPOT];

  if (dirty) { /* summarize local potential energy */
    zcom_util__exit_if (!PAR(cr),
      "node: %d, step: " at_llong_pfmt ": no energy available\n",
        cr->nodeid, step);

#ifdef GMX_MPI
    gmx_sumd(1, &epot, cr);
#endif

  }

  if (MASTER(cr)) {
    atgmx->at->energy = epot;
  }

}



/* only a PP-processor calls this function
 * also assume global_stat() has been called */
int atgmx__move(atgmx_t *atgmx,
    gmx_enerdata_t *enerd,
    at_llong_t step,
    at_bool_t is_first_step,
    at_bool_t is_last_step,
    at_bool_t has_global_stats,
    at_bool_t is_xtc_step,
    at_bool_t is_ns_step,
    t_commrec *cr)
{
  at_bool_t do_tempering, dirty;

  do_tempering = atgmx__do_tempering(atgmx, step, is_ns_step, is_last_step);

  if (!do_tempering) {
    return 0;
  }

  /* no tempering during prerun, temperature is fixed */

  dirty = PAR(cr) && !has_global_stats;

  /* calculate H0, H1 and Ea */
  atgmx__sum_energy(atgmx, enerd->term, cr, step, dirty);

  /* change temperature, and regularly write output files */
  if (MASTER(cr)) {
    at_params_step_t step_params[1];

    step_params->step = (at_llong_t) step;
    step_params->is_first_step = is_first_step;
    step_params->is_last_step = is_last_step;
    step_params->do_log = is_xtc_step;
    step_params->flush_output = AT__FALSE;

    zcom_util__exit_if(0 != at__move(atgmx->at, step_params),
        "#%d, step = " at_llong_pfmt ", error during moving master\n",
        cr->nodeid, step);
  }

  atgmx__update_force_scale(atgmx, cr); /* change scale */

  return 0;
}



void atgmx__scale_force(atgmx_t *atgmx,
    rvec f[], t_mdatoms *mdatoms)
{
  int k;
  real scl;

  if (!atgmx->enabled) {
    return;
  }

  /* scale the force */
  scl = (real) atgmx->at->force_scale;

  for (k = mdatoms->start; k < mdatoms->start + mdatoms->homenr; k++) {
    f[k][0] *= scl;
    f[k][1] *= scl;
    f[k][2] *= scl;
  }
}


#endif
