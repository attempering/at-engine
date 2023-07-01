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

#ifndef AT_EH_RECON_BASIC_H__
#define AT_EH_RECON_BASIC_H__

#include "../at_eh__def.h"
#include "../../utils/at_utils.h"

int at_eh_recon__conf_init(at_eh_recon_t *recon, at_eh_t *eh,
    at_utils_conf_t *conf);

void at_eh_recon__clear(at_eh_recon_t *recon);

void at_eh_recon__finish(at_eh_recon_t *recon);

void at_eh_recon__manifest(const at_eh_recon_t* recon, at_utils_manifest_t *manifest);

#endif
