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

#ifndef AT_MB_SM__SRC_H__
#define AT_MB_SM__SRC_H__


/* simple accumulator */


#include "at_mb_sm.h"


#include "../../../zcom/endn/endn.h"
#include <math.h>


/* scale data points in accumulator by a factor of `f` */
void at_mb_sm__scale(at_mb_sm_t *sm, double f)
{
  sm->s *= f;
  sm->se *= f;
  sm->se2 *= f;
  sm->se3 *= f;
}


/* add a new data point of weight `w` into the accumulator */
void at_mb_sm__add(at_mb_sm_t *sm, double w, double e, int do_third_order)
{
  double s, sp, se, sep, see, seep, de, dep, de2;

  sm->s = sp = (s = sm->s) + w;
  sm->se = sep = (se = sm->se) + e*w;

  if (s <= 0.0) {
    return;
  }

  de = e - se/s;     /* old sum */
  dep = e - sep/sp;  /* new sum */
  sm->se2 = seep = (see = sm->se2) + (de2 = de * dep) * w;

  if (do_third_order) {
    sm->se3 += ((de2 -seep/s) - 2.0*see/s)* dep *w;
  }
}


int at_mb_sm__init(at_mb_sm_t* sm)
{
  if (sm == NULL) {
    fprintf(stderr, "null pointer to at_mb_sm_t\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  at_mb_sm__clear(sm);

  return 0;

ERR:
  return -1;
}


void at_mb_sm__clear(at_mb_sm_t *sm)
{
  sm->s = 0.0;
  sm->se = 0.0;
  sm->se2 = 0.0;
  sm->se3 = 0.0;
}


double at_mb_sm__get_mean(at_mb_sm_t *sm, double min_size)
{
  if (sm->s > min_size) {
    return sm->se / sm->s;
  } else {
    return 0.0;
  }
}


double at_mb_sm__get_var(at_mb_sm_t *sm, double min_size)
{
  if (sm->s > min_size) {
    return sm->se2 / sm->s;
  } else {
    return 0.0;
  }
}



double at_mb_sm__get_std(at_mb_sm_t *sm, double min_size)
{
  return sqrt(at_mb_sm__get_var(sm, min_size));
}



int at_mb_sm__read_binary(at_mb_sm_t *sm, FILE *fp, int endn)
{
  if (sm == NULL) {
    fprintf(stderr, "passing null pointer to at_mb_sm__read_binary\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  /* clear data before reading */
  at_mb_sm__clear(sm);

  /* s */
  if (zcom_endn__fread(&sm->s, sizeof(sm->s), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading sm->s\n");
    goto ERR;
  }
  /* se */
  if (zcom_endn__fread(&sm->se, sizeof(sm->se), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading sm->se\n");
    goto ERR;
  }
  /* se2 */
  if (zcom_endn__fread(&sm->se2, sizeof(sm->se2), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading sm->se2\n");
    goto ERR;
  }
  /* se3 */
  if (zcom_endn__fread(&sm->se3, sizeof(sm->se3), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading sm->se3\n");
    goto ERR;
  }
  return 0;
ERR:
  at_mb_sm__clear(sm);
  return -1;
}



int at_mb_sm__write_binary(at_mb_sm_t *sm, FILE *fp)
{
  if (sm == NULL) {
    fprintf(stderr, "passing null pointer to at_mb_sm__write_binary\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  /* s */
  if (zcom_endn__fwrite(&sm->s, sizeof(sm->s), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing sm->s\n");
    goto ERR;
  }
  /* se */
  if (zcom_endn__fwrite(&sm->se, sizeof(sm->se), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing sm->se\n");
    goto ERR;
  }
  /* se2 */
  if (zcom_endn__fwrite(&sm->se2, sizeof(sm->se2), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing sm->se2\n");
    goto ERR;
  }
  /* se3 */
  if (zcom_endn__fwrite(&sm->se3, sizeof(sm->se3), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing sm->se3\n");
    goto ERR;
  }
  return 0;
ERR:
  return -1;
}



void at_mb_sm__manifest(at_mb_sm_t *sm, FILE *fp, int arrmax)
{
  /* s */
  fprintf(fp, "sm->s: double, %g\n", sm->s);

  /* se */
  fprintf(fp, "sm->se: double, %g\n", sm->se);

  /* se2 */
  fprintf(fp, "sm->se2: double, %g\n", sm->se2);

  /* se3 */
  fprintf(fp, "sm->se3: double, %g\n", sm->se3);

  (void) arrmax;
}


#endif
