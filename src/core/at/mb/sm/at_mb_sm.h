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

#ifndef AT_MB_SM_H__
#define AT_MB_SM_H__


#include "at_mb_sm__def.h"

/* multiply everything in sm by a factor `f` */
void at_mb_sm__scale(at_mb_sm_t *sm, double f);

/* add a new data point of weight `w` to the accumulator */
void at_mb_sm__add(at_mb_sm_t *sm, double w, double e, int do_third_order);

double at_mb_sm__get_mean(at_mb_sm_t *sm, double min_size);

double at_mb_sm__get_var(at_mb_sm_t *sm, double min_size);

double at_mb_sm__get_std(at_mb_sm_t *sm, double min_size);

int at_mb_sm__init(at_mb_sm_t* sm);

void at_mb_sm__clear(at_mb_sm_t *sm);

int at_mb_sm__read_binary(at_mb_sm_t *sm, FILE *fp, int endn);

int at_mb_sm__write_binary(at_mb_sm_t *sm, FILE *fp);

int at_mb_sm__read_text(at_mb_sm_t *sm, FILE *fp);

int at_mb_sm__write_text(at_mb_sm_t *sm, FILE *fp);

void at_mb_sm__manifest(const at_mb_sm_t *sm, FILE *fp, int arrmax);

#endif
