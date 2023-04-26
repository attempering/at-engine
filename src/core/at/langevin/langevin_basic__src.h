#ifndef AT__LANGEVIN_BASIC__SRC_H__
#define AT__LANGEVIN_BASIC__SRC_H__


#include "langevin_basic.h"


#include "../zcom/cfg/cfg.h"



int at_langevin__cfg_init(at_langevin_t *langevin, zcom_cfg_t *cfg, int silent)
{
  /* dt: time step for the temperature Langevin eq */
  langevin->dt = 1e-5;
  if (0 != zcom_cfg__get(cfg, &langevin->dt, "Tdt", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->dt = 1e-5, key: Tdt\n");
  }

  /* dTmax: maximal amount of temperature change in a step */
  langevin->dTmax = 25.0;
  if (0 != zcom_cfg__get(cfg, &langevin->dTmax, "dTmax", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->dTmax = 25.0, key: dTmax\n");
  }

  /* whether to apply the Metropolisation correction */
  langevin->corrected = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_corrected", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->corrected = 1, key: langevin_corrected\n");
  }

  /* whether to avoid crossing over unvisited bins */
  langevin->no_skip = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->corrected, "langevin_no_skip", "%d")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->no_skip = 1, key: langevin_no_skip\n");
  }

  /* minimum number of visits before moving out of a bin */
  langevin->bin_min_visits = 1;
  if (0 != zcom_cfg__get(cfg, &langevin->bin_min_visits, "langevin_bin_min_visits", "%lf")) {
    if (!silent) fprintf(stderr, "assuming default: langevin->bin_min_visits = 1, key: langevin_bin_min_visits\n");
  }

  /* rejects: number of rejected Langevin moves */
  langevin->rejects = 0.0;
  /* total: total number of Langevin moves */
  langevin->total = 0.0;

  langevin->integrate_func = NULL;

  return 0;
}


void at_langevin__clear(at_langevin_t *langevin)
{
  langevin->rejects = 0.0;
  langevin->total = 0.0;
}


void at_langevin__finish(at_langevin_t *langevin)
{
  memset(langevin, 0, sizeof(*langevin));
}


#endif
