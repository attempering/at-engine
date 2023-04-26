#ifndef AT__MB_BETADIST__SRC_H__
#define AT__MB_BETADIST__SRC_H__

/* basic routines for the multiple-bin estimators */

#include "mb_betadist.h"
#include "../mb_basic.h"

#include <stdlib.h>

#include "../../zcom/cfg/cfg.h"



int mb_betadist__cfg_init(mb_betadist_t *betadist, cfg_t *cfg, mb_t *mb)
{
  int i;

  betadist->beta_min = mb->bmin;
  betadist->beta_max = mb->bmax;
  betadist->n = mb->n;

  /* ens_exp: ensemble exponent of beta */
  betadist->ens_exp = 1.0;
  if (0 != cfg_get(cfg, &betadist->ens_exp, "ensemble_factor", "%lf")) {
    fprintf(stderr, "assuming default: betadist->ens_exp = 1.0, key: ensemble_factor\n");
  }

  /* flat ensemble mode */
  betadist->mode = 0;
  if (0 != cfg_get(cfg, &betadist->mode, "ensemble_mode", "%d"))
    fprintf(stderr, "assuming default: betadist->mode = 0, key: ensemble_mode\n");

  /* default values */
  betadist->beta0 = 0.5 * (mb->bmax + mb->bmin);
  betadist->invsigma2 = 1.0;
  betadist->c = 0.0;

  if(betadist->mode == 1)
  {
    /* beta0 */
    if (0 != cfg_get(cfg, &betadist->beta0, "ensemble_beta0", "%lf"))
      fprintf(stderr, "assuming default: betadist->beta0 = 0.5 * (mb->bmax + mb->bmin), key: ensemble_beta0\n");
    if (betadist->beta0 >= mb->bmax || betadist->beta0 <= mb->bmin)
      fprintf(stderr, "WARNING: beta0 is not in the temperature range!\n");

    /* sigma */
    double sigma = 1.0;
    if (0 != cfg_get(cfg, &sigma, "ensemble_sigma", "%lf"))
      fprintf(stderr, "assuming default: mb->sigma = 1.0, key: ensemble_sigma\n");
    if (sigma == 0)
    {
      fprintf(stderr, "ERROR: sigma cannot be zero!\n");
      goto ERR;
    }
    betadist->invsigma2 = 1.0/sigma/sigma;
  }
  else if(betadist->mode == 2)
  {
    /* c */
    betadist->c = 0.0;
    if (0 != cfg_get(cfg, &betadist->c, "ensemble_c", "%lf"))
      fprintf(stderr, "assuming default: betadist->c = 0.0, key: ensemble_c\n");
  }
  else if(betadist->mode != 0)
  {
    fprintf(stderr, "invalid multicanonical ensemble parameter\n");
    goto ERR;
  }

  /* ens_w: array of ensemble weights at bin boundaries */
  if ((betadist->ens_w = (double *) calloc((betadist->n + 2), sizeof(double))) == NULL) {
    fprintf(stderr, "no memory! var: mb->ens_w, type: double\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }

  for (i = 0; i < betadist->n+1; i++) {
    double invw = mb_betadist__calc_inv_weight(mb->betadist, mb->barr[i], NULL, NULL, NULL);
    betadist->ens_w[i] = 1.0/invw;
  }

  return 0;

ERR:
  return 1;
}


void mb_betadist__manifest(const mb_betadist_t *betadist, FILE *fp, int arrmax)
{
  int i, pacnt;

  /* ens_exp: ensemble exponent of beta */
  fprintf(fp, "mb->ens_exp: double, %g\n", betadist->ens_exp);

  /* mode */
  fprintf(fp, "mb->mode: int, %d\n", betadist->mode);
  if(betadist->mode == 1)
  {
    /* beta0 */
    fprintf(fp, "mb->beta0: double, %g\n", betadist->beta0);

    /* invsigma2 */
    fprintf(fp, "mb->invsigma2: double, %g\n", betadist->invsigma2);
  }
  else if(betadist->mode == 2)
  {
    /* c */
    fprintf(fp, "mb->c: double, %g\n", betadist->c);
  }

  /* ens_w: array of ensemble weights at bin boundaries */
  fprintf(fp, "mb->ens_w: dynamic array of mb->n+1: ");
  for (i = betadist->n+1-1; i >= 0; i--) {
    if (fabs(betadist->ens_w[i]) > 1e-30) {
      break;
    }
  }

  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && betadist->n+1 > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < betadist->n+1; i++) {
      if (i == arrmax && i < betadist->n+1-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (betadist->n+1-arrmax)) {
        continue;
      }
      fprintf(fp, "%g, ", betadist->ens_w[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }
}



void mb_betadist__finish(mb_betadist_t *betadist)
{
  if (betadist->ens_w != NULL) {
    free(betadist->ens_w);
  }
}



/* compute the ensemble f factor
 * f: f(beta);
 * *pndfdbeta: -df(beta)/dbeta;
 */
static double mb_betadist_calc_f_factor(mb_betadist_t *betadist, double beta, double *pndfdbeta)
{
  double f, ndfdbeta;

  /* invwf: 1/w(beta)/f(beta);
   * f: f(beta);
   * ndfdbeta: -df(beta)/dbeta;
   */

  if (betadist->mode == 1)
  {
    double beta_diff = beta - betadist->beta0;
    f = exp(-0.5 * (beta_diff * beta_diff) * betadist->invsigma2);
    ndfdbeta = f * beta_diff * betadist->invsigma2;
  }
  else if (betadist->mode == 2)
  {
    f = exp(-beta * betadist->c);
    ndfdbeta = f * betadist->c;
  }
  else
  {
    f = 1.0;
    ndfdbeta = 0.0;
  }

  if (pndfdbeta != NULL) {
    *pndfdbeta = ndfdbeta;
  }

  return f;
}


static double mb_betadist_calc_invw_factor(mb_betadist_t *betadist, double beta)
{
  const double eps = 1e-5;
  double beta_r = beta / betadist->beta_max; /* to relative beta */

  int ifac = (int) (betadist->ens_exp + 0.5); /* round to nearest int */

  double invw;

  if (fabs(betadist->ens_exp - ifac) < eps && ifac >= 0) {
    for (invw = 1.0; ifac > 0; ifac--) {
      invw *= beta_r;
    }
  } else { /* unable to avoid exponential */
    invw = exp(betadist->ens_exp * log(beta));
  }

  return invw;
}


double mb_betadist__calc_inv_weight(mb_betadist_t *betadist, double beta,
    double *neg_dlnwf_dbeta, double *pf, double *neg_df_dbeta)
{
  double f, invw, invwf, neg_df_dbeta_ = 0.0;

  if (neg_df_dbeta == NULL) {
    neg_df_dbeta = &neg_df_dbeta_;
  }

  f = mb_betadist_calc_f_factor(betadist, beta, neg_df_dbeta);

  if (pf != NULL) {
    *pf = f;
  }

  // beta^{-ens_exp}
  invw = mb_betadist_calc_invw_factor(betadist, beta);

  invwf = invw / f;

  exit_if (invwf > 1e6 || invwf < 1e-6, "bad invwf=%g, beta=%g\n", invwf, beta);

  if (neg_dlnwf_dbeta != NULL) {
    *neg_dlnwf_dbeta = (betadist->ens_exp / beta)
                     + (*neg_df_dbeta / f);
  }

  return invwf;
}


#endif
