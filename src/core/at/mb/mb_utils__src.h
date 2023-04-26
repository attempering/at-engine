#ifndef AT__MB_UTILS__SRC_H__
#define AT__MB_UTILS__SRC_H__

#include "mb_basic.h"
#include "iie/mb_iie.h"
#include "accum/mb_accum.h"
#include "mb_utils.h"

#include "../zcom/util/util.h"
#include "../zcom/endn/endn.h"




int mb__write_ze_file(mb_t *mb, const char *fname)
{
  int i, ip;
  FILE *fp;

  if (fname == NULL) {
    fname = mb->ze_file;
  }

  exit_if (fname == NULL, "file name is NULL");

  mb_accum__calc_win_total(mb->accum);

  mb_iie_gridvals__calc(mb);

  if ((fp = fopen(fname, "w")) == NULL) {
    fprintf(stderr, "cannot open %s.\n", fname);
    return 1;
  }

  for (i = 0; i <= mb->n; i++) {
    fprintf(fp, "%16.10f %20.10f %22.10f %22.10f ",
      mb->barr[i], // 1
      mb->iie->gridvals->items[i].lnz, // 2
      mb->iie->gridvals->items[i].e, // 3
      mb->iie->gridvals->items[i].cv); // 4

    ip = (i < mb->n) ? i : (i-1); /* for quantities with no [mb->n] */

    fprintf(fp, " %22.10f %s %+10.6f %22.10e %22.10e %22.10e %22.10e",
      mb->iie->et->items[ip].value, // 5
      mb_iie_gridvals_item__quality_bits_to_string(&mb->iie->gridvals->items[i]), // 6 
      0.0, // mb->iie->imbal[ip], // 7
      mb->accum->sums[ip].s, // 8
      mb->visits[ip], // 9
      mb->shk->window_multiplier[ip], // 10
      mb->accum->win_total[ip]);

    fprintf(fp, "\n");
  }

  fclose(fp);

  return 0;
}


int mb__beta_to_index(mb_t *mb, double beta, int check)
{
  int j;

  if (beta > mb->bmin) {
    j = (int)((beta - mb->bmin)/mb->bdel);
  } else {
    j = -1;
  }

  if (check) {
    exit_if (j < 0 || j >= mb->n, "beta = %d, %g, range = (%g, %g, %g)\n",
        j, beta, mb->bmin, mb->bdel, mb->bmax);
  }

  return j;
}


void mb__add(mb_t *mb, double e, double beta,
    int *pib, double *pinvwf, double *neg_dlnwf_dbeta)
{
  double invwf, f = 1.0, neg_df_dbeta = 0.0;
  int bCv = mb->flags & MB_CV;

  int j = mb__beta_to_index(mb, beta, 1);

  *pib = j;

  mb->visits[j] += 1.0;
  mb->total_visits += 1.0;

  /* calculate the inverse ensemble weight
   *
   * f: f(beta);
   * neg_df_dbeta: -df/dbeta;
   * invwf: 1/w(beta)/f(beta);
   * neg_dlnwf_dbeta: -dln(w(beta)f(beta))/dbeta;
   * ginvwf: adaptive weight = ampf * invwf;
   */
  invwf = mb_betadist__calc_inv_weight(mb->betadist, beta, neg_dlnwf_dbeta, &f, &neg_df_dbeta);

  if (pinvwf != NULL) {
    *pinvwf = invwf;
  }

  mb_accum__add(mb->accum, j, invwf, e, bCv, mb->shk, mb->total_visits);

}



void mb__refresh_et(mb_t *mb, int reps)
{
  int i, rep;

  for (rep = 0; rep < reps; rep++) {
    for (i = 0; i < mb->n; i++)
      mb_iie_et__calc_et(mb->iie, i);
  }
}



#endif
