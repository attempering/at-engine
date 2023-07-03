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

#ifndef AT_UTILS_MODSTACK__SRC_H__
#define AT_UTILS_MODSTACK__SRC_H__

#include "at_utils_modstack.h"


void at_utils_modstack__init(
    at_utils_modstack_t *mods)
{
  mods->curr = 0;
  mods->stack[0] = NULL;
}


void at_utils_modstack__finish(
    at_utils_modstack_t *mods)
{
  mods->curr = 0;
}


const char *at_utils_modstack__push(
    at_utils_modstack_t *mods,
    const char *name)
{
  if (mods->curr >= AT_UTILS_MODSTACK__STACK_MAX - 1) {
    fprintf(stderr, "Error@at.utils.modstack: stack overflow for %s\n",
        name);
    return NULL;
  }

  mods->stack[++mods->curr] = name;

  return name;
}


const char *at_utils_modstack__pop(
    at_utils_modstack_t *mods)
{
  const char *name = NULL;

  if (mods->curr <= 0) {
    fprintf(stderr, "Error@at.utils.modstack: stack@%d underflow for %s\n",
        mods->curr, (name ? name : "(null)"));
    return NULL;
  }

  name = mods->stack[mods->curr--];

  return name;
}


const char *at_utils_modstack__get(
    const at_utils_modstack_t *mods)
{
  if (mods->curr > 0 && mods->curr < AT_UTILS_MODSTACK__STACK_MAX) {
    return mods->stack[mods->curr];
  }

  return NULL;
}


#endif
