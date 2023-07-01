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

#include "at/distr/at_distr__src.h"
#include "at/utils/at_utils__src.h"
#include "zcom/zcom__src.h"

int main(void)
{
  at_distr_t distr[1];
  at_utils_conf_t conf[1];
  at_utils_manifest_t manifest[1];
  double boltz = 1.0;
  at_bool_t verbose = AT__TRUE;

  at_utils_conf__init_ez(conf, "at.cfg", "atdata", verbose);

  at_utils_manifest__conf_init(manifest, conf);
  at_utils_manifest__open_file(manifest);

  at_distr__conf_init(distr, conf, boltz);
  at_distr__manifest(distr, manifest);
  at_distr__finish(distr);

  at_utils_manifest__finish(manifest);
  at_utils_conf__finish_ez(conf);

  return 0;
}