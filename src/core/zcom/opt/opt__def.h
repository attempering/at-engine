/* 
 * Copyright (C) 2006-2023  AT-Engine Developers
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

#ifndef ZCOM__OPT__DEF_H__
#define ZCOM__OPT__DEF_H__


#include <stdio.h>

#include "../def/def.h"
#include "../utils/utils.h"

/* option either from arguments or configuration */
typedef struct zcom_opt_t_ {
  int isopt; /* is option or argument */
  char ch; /* single letter option flag */
  const char *sflag; /* long string flag */
  const char *key; /* key */

  const char *val; /* raw string from command line */
  const char *desc; /* description */
  const char *fmt; /* sscanf format */
  const char *pfmt; /* printf format, NULL: to guess */
  void *ptr; /* address to the target variable */
  unsigned flags;
} zcom_opt_t;

#define ZCOM_OPT__MANDATORY     0x0001  /* a mandatory argument or option */
#define ZCOM_OPT__SWITCH        0x0002  /* an option is a switch */
#define ZCOM_OPT__SET           0x0004  /* an argument/option is set */

#endif
