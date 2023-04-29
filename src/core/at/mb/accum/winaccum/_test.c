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

// gcc -g _test.c -lm && valgrind ./a.out
#include "../../../zcom/zcom__src.h"


#include "../../../distr/at_distr__src.h"
#include "../../sm/mb_sm__src.h"
#include "../../win/mb_win__src.h"
#include "../../shk/mb_shk__src.h"
#include "mb_accum_winaccum__src.h"



static void mock_mb__init(at_distr_t *distr, at_mb_t *mb, double bmin, double bmax, double bdel)
{
  int i, n;

  at_distr__cfg_init(distr, NULL, boltz, 1);
  at_distr_domain__init_simple(distr->domain, bmin, bmax, bdel);

  mb->distr = distr;

  mb->flags = MB_SYMWIN | MB_USE_WIN_ACCUM;
}


static void mock_mb__finish(at_mb_t *mb)
{
  at_distr__finish(mb->distr);
}



int main()
{
  at_distr_t distr[1];
  at_mb_accum_winaccum_t winaccum[1];
  at_mb_win_t win[1];
  at_mb_t mb[1];


  mock_mb__init(distr, mb, 0.2, 0.4, 0.001);

  at_mb_win__cfg_init(win, NULL, mb);
  at_mb_accum_winaccum__init(winaccum, mb->distr->domain->n, win, 0);

  at_mb_accum_winaccum__finish(winaccum);
  at_mb_win__finish(win);

  mock_mb__finish(mb);

  return 0;
}