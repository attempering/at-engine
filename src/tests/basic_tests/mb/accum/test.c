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

#include "at/utils/at_utils__src.h"
#include "at/distr/at_distr__src.h"
#include "at/mb/sm/at_mb_sm__src.h"
#include "at/mb/win/at_mb_win__src.h"
#include "at/mb/shk/at_mb_shk__src.h"
#include "at/mb/accum/at_mb_accum__src.h"
#include "zcom/zcom__src.h"



static void mock_mb__init(at_utils_conf_t *conf,
    at_distr_t *distr, at_mb_t *mb, double beta_min, double beta_max, double beta_del)
{
  int i, n;
  double boltz = 1.0;

  at_distr__conf_init(distr, conf, boltz);
  at_distr_domain__init_simple(distr->domain, beta_min, beta_max, beta_del);

  mb->distr = distr;

  mb->use_sym_wins = AT__TRUE;
  mb->accum->winaccum->enabled = AT__TRUE;
}


static void mock_mb__finish(at_mb_t *mb)
{
  at_distr__finish(mb->distr);
}



int main()
{
  at_utils_conf_t conf[1];
  at_distr_t distr[1];
  at_mb_accum_t accum[1];
  at_mb_win_t win[1];
  at_mb_t mb[1];

  at_utils_conf__init_ez(conf, "at.cfg", "atdata", AT__FALSE);

  mock_mb__init(conf, distr, mb, 0.2, 0.4, 0.001);

  at_mb_win__conf_init(win, conf, mb);
  at_mb_accum__conf_init(accum, mb->distr->domain->n, win, conf);

  at_mb_accum__finish(accum);
  at_mb_win__finish(win);

  mock_mb__finish(mb);

  return 0;
}