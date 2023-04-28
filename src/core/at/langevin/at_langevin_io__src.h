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

#ifndef AT_LANGEVIN_IO__SRC_H__
#define AT_LANGEVIN_IO__SRC_H__

#include "at_langevin_io.h"

#include "../../zcom/zcom.h"

void at_langevin__manifest(const at_langevin_t *langevin, at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

  fprintf(fp, "langevin->dt: double, %g\n", langevin->dt);

  fprintf(fp, "langevin->dTmax: double, %g\n", langevin->dTmax);

  fprintf(fp, "langevin->rejects: double, %g\n", langevin->rejects);

  fprintf(fp, "langevin->total: double, %g\n", langevin->total);

  fprintf(fp, "langevin->corrected: int, %d\n", langevin->corrected);

  fprintf(fp, "langevin->no_skip: int, %d\n", langevin->no_skip);

  fprintf(fp, "langevin->bin_min_visits: double, %lf\n", langevin->bin_min_visits);

  fprintf(fp, "langevin->integrate_func: %p\n", langevin->integrate_func);
}



int at_langevin__read_binary(at_langevin_t *langevin, FILE *fp, int endn)
{
  /* rate */
  double rate = 0.0;
  if (zcom_endn__fread(&rate, sizeof(rate), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading rate\n");
    goto ERR;
  }

  /* total: total number of attempts of using langevin equation */
  if (zcom_endn__fread(&langevin->total, sizeof(langevin->total), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading langevin->total\n");
    goto ERR;
  }
  if ( !((langevin->rejects=langevin->total*rate) >= 0.0) ) {
    fprintf(stderr, "langevin->total: failed validation: (langevin->rejects=langevin->total*rate) >= 0.0\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  return 0;

ERR:
  return 1;
}



int at_langevin__write_binary(at_langevin_t *langevin, FILE *fp)
{
  /* rate */
  double rate = (langevin->total > 1.0) ? (langevin->rejects/langevin->total) : 0.0;
  if (zcom_endn__fwrite(&rate, sizeof(rate), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing langevin->rate\n");
    goto ERR;
  }

  /* total: total number of attempts of using langevin equation */
  if (zcom_endn__fwrite(&langevin->total, sizeof(langevin->total), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing langevin->total\n");
    goto ERR;
  }

  return 0;

ERR:

  return 1;
}

#endif