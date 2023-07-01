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

#include "at-engine.h"

at_bool_t at_mb__equal(const at_mb_t *mb1, const at_mb_t *mb2)
{
  if (fabs(mb1->total_visits - mb2->total_visits) > 1e-3) {
    fprintf(stderr, "mb->total_visits mismatch, %g vs %g\n",
        mb1->total_visits, mb2->total_visits);
    return AT__FALSE;
  }

  if (mb1->accum->winaccum->enabled != mb2->accum->winaccum->enabled) {
    fprintf(stderr, "mb->accum->winaccum->enabled mismatch, %d vs %d\n",
        mb1->accum->winaccum->enabled, mb2->accum->winaccum->enabled);
    return AT__FALSE;
  }

  if (mb1->use_single_bin != mb2->use_single_bin) {
    fprintf(stderr, "mb->use_single_bin mismatch, %d vs %d\n",
        mb1->use_single_bin, mb2->use_single_bin);
    return AT__FALSE;
  }

  if (mb1->use_sym_wins != mb2->use_sym_wins) {
    fprintf(stderr, "mb->use_sym_wins mismatch, %d vs %d\n",
        mb1->use_sym_wins, mb2->use_sym_wins);
    return AT__FALSE;
  }

  if (mb1->distr->domain->n != mb2->distr->domain->n) {
    fprintf(stderr, "domain->n mismatch, %d vs %d\n",
        mb1->distr->domain->n, mb1->distr->domain->n);
    return AT__FALSE;
  }

  if (mb1->shk->max != mb2->shk->max) {
    fprintf(stderr, "mb->shk->max mismatch, %g vs %g\n",
        mb1->shk->max, mb2->shk->max);
    return AT__FALSE;
  }

  if (mb1->win->max_win_bins != mb2->win->max_win_bins) {
    fprintf(stderr, "mb->win->max_win_bins mismatch, %d vs %d\n",
        mb1->win->max_win_bins, mb2->win->max_win_bins);
    return AT__FALSE;
  }

  fprintf(stderr, "no difference found.\n");

  return AT__TRUE;
}
