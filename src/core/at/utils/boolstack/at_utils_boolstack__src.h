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

#ifndef AT_UTILS_BOOLSTACK__SRC_H__
#define AT_UTILS_BOOLSTACK__SRC_H__

#include "at_utils_boolstack.h"



static const char *at_utils_boolstack__bool_to_str(
    at_bool_t value)
{
  return value ? "true" : "false";
}


void at_utils_boolstack__init(
    at_utils_boolstack_t *st,
    at_bool_t value)
{
  st->curr = 0;

  // Note: the level-0 store the default value,
  // which may never be popped
  st->stack[0] = value;
}


void at_utils_boolstack__finish(
    at_utils_boolstack_t *st)
{
  st->curr = 0;
}


at_bool_t at_utils_boolstack__push(
    at_utils_boolstack_t *st,
    at_bool_t value)
{
  if (st->curr >= AT_UTILS_BOOLSTACK__STACK_MAX - 1) {
    fprintf(stderr, "Error@at.utils.boolstack: stack overflow for %s\n",
        at_utils_boolstack__bool_to_str(value));
    return st->stack[AT_UTILS_BOOLSTACK__STACK_MAX - 1];
  }

  st->stack[++st->curr] = value;

  return value;
}


at_bool_t at_utils_boolstack__pop(
    at_utils_boolstack_t *st)
{
  at_bool_t value;

  if (st->curr <= 0) {
    fprintf(stderr, "Error@at.utils.boolstack: stack@%d underflow for %s\n",
        st->curr, at_utils_boolstack__bool_to_str(st->stack[0]));
    return st->stack[0];
  }

  value = st->stack[st->curr--];

  return value;
}


at_bool_t at_utils_boolstack__get(
    const at_utils_boolstack_t *st)
{
  if (st->curr > 0 && st->curr < AT_UTILS_BOOLSTACK__STACK_MAX) {
    return st->stack[st->curr];
  }

  return st->stack[0];
}



#endif
