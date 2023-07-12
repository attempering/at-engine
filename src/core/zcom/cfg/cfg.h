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

#ifndef ZCOM__CFG_H__
#define ZCOM__CFG_H__

#include "cfg__def.h"

zcom_cfg_t *zcom_cfg__open(const char *fn, zcom_ssm_t *ssm, unsigned flags);
void zcom_cfg__close(zcom_cfg_t *cfg);
int zcom_cfg__add(zcom_cfg_t *cfg, const char *key, const char *fmt, void *ptr, const char *desc);
int zcom_cfg__match(zcom_cfg_t *cfg, unsigned flags);
int zcom_cfg__get(zcom_cfg_t *cfg, void *var, const char *key, const char *fmt);

#endif
