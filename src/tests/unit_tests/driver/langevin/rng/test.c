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

//#define AT_MB_DBG__ 1
#include "at-engine__src.h"


static void init_distr_mb_langevin_objects(at_utils_conf_t *conf,
    at_distr_t *distr, at_mb_t *mb, at_driver_langevin_t *langevin)
{
  at_distr__conf_init_ez_(distr, conf);

  at_mb__conf_init_ez_(mb, distr, conf);

  at_driver_langevin__conf_init(langevin, distr, mb, conf, 0);
}



static void test(at_driver_langevin_t *langevin, at_utils_conf_t *conf)
{
  uint32_t seed = 12345;

  //langevin->rng->file = zcom_ssm__dup(ssm, "rng.dat");
  //langevin->rng->mtrng = NULL;

  at_driver_langevin_rng__reset(langevin->rng, seed, conf->ssm, conf->data_dir, "rng-reset.dat");

}



int main(void)
{
  at_utils_conf_t conf[1];
  at_bool_t verbose = AT__TRUE;
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_driver_langevin_t langevin[1];
  at_bool_t passed = AT__TRUE;

  at_utils_conf__init_ez(conf, "at.cfg", "atdata", verbose);
  init_distr_mb_langevin_objects(conf, distr, mb, langevin);

  test(langevin, conf);

  at_distr__finish(mb->distr);
  at_mb__finish(mb);
  at_driver_langevin__finish(langevin);

  at_utils_conf__finish_ez(conf);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
