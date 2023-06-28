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


double boltz = 1.0;

const char *data_dir = "data";

zcom_ssm_t *init_distr_mb_langevin_objects(at_distr_t *distr, at_mb_t *mb, at_driver_langevin_t *langevin)
{
  zcom_cfg_t *cfg = NULL;
  //zcom_cfg__open("at.cfg", ZCOM_CFG__IGNORE_CASE | ZCOM_CFG__ALLOW_DASHES);
  at_bool_t verbose = AT__FALSE;

  zcom_ssm_t *ssm = zcom_ssm__open();

  at_distr__cfg_init(distr, cfg, boltz, verbose);

  at_mb__cfg_init(mb, distr, cfg, boltz, ssm, data_dir, verbose);

  at_driver_langevin__cfg_init(langevin, distr, mb, cfg, ssm, data_dir, verbose);

  zcom_cfg__close(cfg);

  return ssm;
}



void test(at_driver_langevin_t *langevin, zcom_ssm_t *ssm)
{
  uint32_t seed = 12345;

  //langevin->rng->file = zcom_ssm__dup(ssm, "rng.dat");
  //langevin->rng->mtrng = NULL;

  at_driver_langevin_rng__reset(langevin->rng, seed, ssm, data_dir, "rng-reset.dat");

}



int main(int argc, char **argv)
{
  at_distr_t distr[1];
  at_mb_t mb[1];
  at_driver_langevin_t langevin[1];
  at_bool_t passed = AT__TRUE;
  zcom_ssm_t *ssm;

  ssm = init_distr_mb_langevin_objects(distr, mb, langevin);

  test(langevin, ssm);

  //at_distr__finish(mb->distr);
  //at_mb__finish(mb);
  //at_driver_langevin__finish(langevin);

  zcom_ssm__close(ssm);

  if (passed) {
    printf("Passed.\n");
  } else {
    printf("Failed.\n");
  }

  return 0;
}
