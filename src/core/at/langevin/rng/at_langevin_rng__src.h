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

#ifndef AT_LANGEVIN_RNG__SRC_H__
#define AT_LANGEVIN_RNG__SRC_H__

#include "at_langevin_rng.h"

#include "../../../zcom/zcom.h"



void at_langevin_rng__reset(at_langevin_rng_t *rng, uint32_t seed)
{
  rng->mtrng = zcom_mtrng__open(seed);
  zcom_mtrng__load_or_init_from_seed(rng->mtrng, rng->filename, seed);
}


void at_langevin_rng__save(at_langevin_rng_t *rng)
{
  if (rng->mtrng) {
    zcom_mtrng__save(rng->mtrng, rng->filename);
  }
}


void at_langevin_rng__cfg_init(at_langevin_rng_t *rng,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    int silent)
{
  rng->filename = "rng.dat";
  if (cfg != NULL && zcom_cfg__get(cfg, &rng->filename, "rng_file", "%s"))
  {
    fprintf(stderr, "assuming default: langevin->rng->filename = \"rng.dat\", key: rng_file\n");
  }

  rng->filename = at_utils__make_output_filename(ssm, data_dir, rng->filename);

  rng->mtrng = NULL;

  rng->inited = 1;
}



void at_langevin_rng__finish(at_langevin_rng_t *rng)
{
  if (rng->inited) {
    if (rng->mtrng != NULL) {
      zcom_mtrng__save(rng->mtrng, rng->filename);
      zcom_mtrng__close(rng->mtrng);
    }
  }
}

void at_langevin_rng__manifest(at_langevin_rng_t *rng,
    at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  fprintf(fp, "rng->filename: char *, %s\n", rng->filename);
}


#endif
