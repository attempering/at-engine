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

// gcc -g _test.c -lm && valgrind ./a.out
#include "../../../zcom/util/util__src.h"
#include "../../../zcom/ssm/ssm__src.h"
#include "../../../zcom/endn/zcom_endn__src.h"
#include "../../../zcom/opt/opt__src.h"
#include "../../../zcom/cfg/cfg__src.h"


#include "../../sm/mb_sm__src.h"
#include "../../win/mb_win__src.h"
#include "../../betadist/mb_betadist__src.h"
#include "../../shk/mb_shk__src.h"
#include "mb_accum_winaccum__src.h"



static void mock_mb__init(at_mb_t *mb, double bmin, double bmax, double bdel)
{
  int i, n;

  n = (int) ((bmax - bmin)/bdel + 0.5);
  mb->n = n;
  mb->bmin = bmin;
  mb->bmax = bmax;
  mb->bdel = bdel;
  mb->barr = calloc(n+1, sizeof(double));

  for (i = 0; i <= n; i++) {
    mb->barr[i] = bmin + bdel * i;
  }

  mb->flags = MB_SYMWIN | MB_USE_WIN_ACCUM;
}


static void mock_mb__finish(at_mb_t *mb)
{
  free(mb->barr);
}



int main()
{
  at_mb_accum_winaccum_t winaccum[1];
  at_mb_win_t win[1];
  at_mb_t mb[1];


  mock_mb__init(mb, 0.2, 0.4, 0.001);

  at_mb_win__cfg_init(win, NULL, mb);
  at_mb_accum_winaccum__init(winaccum, mb->n, win, 0);

  at_mb_accum_winaccum__finish(winaccum);
  at_mb_win__finish(win);

  mock_mb__finish(mb);

  return 0;
}