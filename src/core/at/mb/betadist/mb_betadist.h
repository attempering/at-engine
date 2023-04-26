#ifndef AT__MB_BETADIST_H__
#define AT__MB_BETADIST_H__

#include <stdio.h>

#include "mb_betadist__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;

int at_mb_betadist__cfg_init(at_mb_betadist_t *betadist, zcom_cfg_t *cfg, at_mb_t *mb);

void at_mb_betadist__finish(at_mb_betadist_t *betadist);

void at_mb_betadist__manifest(const at_mb_betadist_t *betadist, FILE *fp, int arrmax);

/* return the inverse of overall temperature weight
 *
 * The weight is a product of two components, f and w factor
 * 
 * The f factor is 1 if mb->mode == 0,
 * or a Gaussian in the beta space if mb->mode == 1.
 * 
 * The w factor is proportional to beta^{ - mb->ens_exp }. 
 * 
 * The return value is the inverse of the product of the two factors.
 * 
 * The value of the f factor and its negative derivative w.r.t. beta
 * are saved in *p_f and *p_neg_df_dbeta, respectively,
 * if the two pointers are not NULL.
 */
double at_mb_betadist__calc_inv_weight(at_mb_betadist_t *betadist, double beta,
    double *neg_dlnwf_dbeta, double *f, double *neg_df_dbeta);

#endif
