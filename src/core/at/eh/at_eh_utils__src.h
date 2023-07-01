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

#ifndef AT_EH_UTILS__SRC_H__
#define AT_EH_UTILS__SRC_H__

#include "at_eh_utils.h"

#include "../distr/at_distr.h"
#include <stdio.h>
#include <math.h>


void at_eh__add(at_eh_t *eh, int ib, double e)
{
  if (eh->mode > 0) { /* add to energy histogram */

    int ie = (int)((e - eh->e_min)/eh->e_del);

    if (e > eh->e_max_runtime) {
      fprintf(stderr, "\rWarning@at: at->eh: energy point overflow %g, (%g, %g)\n",
          e, eh->e_min_runtime, eh->e_max_runtime);
      eh->e_max_runtime = e;
    } else if (e < eh->e_min_runtime) {
      fprintf(stderr, "\rWarning@at: at->eh: energy point underflow %g, (%g, %g)\n",
          e, eh->e_min_runtime, eh->e_max_runtime);
      eh->e_min_runtime = e;
    }

    if (ie >= 0 && ie < eh->e_n) {
      eh->his[ib*eh->e_n+ie] += 1.0;
    }

  }
}


int at_eh__reconstruct(at_eh_t *eh, const char *fn)
{
  return at_eh_recon__reconstruct(eh->recon, eh, fn);
}

#endif
