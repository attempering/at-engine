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

#ifndef ZCOM__CFG__DEF_H__
#define ZCOM__CFG__DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../def/def.h"
#include "../opt/opt.h"
#include "../ssm/ssm.h"


typedef struct {
  char *key, *val;
  int used;
} zcom_cfgent_t; /* line from cfg file */


typedef struct zcom_cfg_t_ {
  char *buf;      /* the string buffer of the entire configuration file */
  int nent;       /* number of entries */
  zcom_cfgent_t *ents; /* entries */
  int nopt;       /* number of user-requested options */
  int nopt_cap;   /* capacity of opts[] */
  unsigned flags;
  zcom_opt_t *opts;    /* user-requested options */
  zcom_ssm_t *ssm;
} zcom_cfg_t;



/* ignoring cases */
#define ZCOM_CFG__IGNORE_CASE       0x0010

/* treating '_' as the same as '-' */
#define ZCOM_CFG__ALLOW_DASHES      0x0020

/* ignore '_' and '-' */
#define ZCOM_CFG__IGNORE_DASHES     0x0040

#define ZCOM_CFG__CHECK_USE         0x0100
#define ZCOM_CFG__VERBOSE           0x1000
#define ZCOM_CFG__OPT_BLOCK_SIZE_   64

#endif
