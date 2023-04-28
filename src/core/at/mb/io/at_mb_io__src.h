#ifndef AT_MB_IO__SRC_H__
#define AT_MB_IO__SRC_H__



/* IO routines for at_mb_t */

#include "at_mb_io.h"

/* implementation dependent headers */
#include "../at_mb_basic.h"

/* include the source code of sub-modules */
#include "at_mb_io_binary__src.h"




int at_mb__read(at_mb_t *mb, at_langevin_t *langevin, double *beta)
{
  int ret, version;

  ret = at_mb__read_binary(mb, langevin, beta, mb->av_file, &version);

  if (ret != 0) {
    fprintf(stderr, "cannot load at_mb_t data from %s\n", mb->av_file);
    return 1;
  }

  fprintf(stderr, "loaded previous at_mb_t data, %s version: %d\n",
    mb->av_file, version);

  return 0;
}



int at_mb__write(at_mb_t *mb, at_langevin_t *langevin, double beta)
{
  return at_mb__write_binary(mb, langevin, beta, mb->av_file, 1);
}



void at_mb__manifest(const at_mb_t *mb, FILE *fp, int arrmax)
{
  int i;
  int pacnt;

  /* bdel: bin size of beta */
  fprintf(fp, "mb->bdel: double, %g\n", mb->bdel);

  /* n: number of temperature bins */
  fprintf(fp, "mb->n: int, %4d\n", mb->n);

  /* barr: temperature array */
  fprintf(fp, "mb->barr: dynamic array of mb->n+1: ");
  for (i = mb->n+1-1; i >= 0; i--) if (fabs(mb->barr[i]) > 1e-30) break;
  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && mb->n+1 > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < mb->n+1; i++) {
      if (i == arrmax && i < mb->n+1-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (mb->n+1-arrmax)) continue;
      fprintf(fp, "%g, ", mb->barr[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

  /* flags: combination of flags */
  fprintf(fp, "mb->flags: unsigned, 0x%X\n", mb->flags);

  /* MB_USE_WIN_ACCUM: use adaptive averaging */
  fprintf(fp, "mb->flags & MB_USE_WIN_ACCUM (0x%X, mbest_damp): %s\n",
    MB_USE_WIN_ACCUM, (mb->flags & MB_USE_WIN_ACCUM) ? "on" : "off");

  /* MB_CV: compute heat capacity */
  fprintf(fp, "mb->flags & MB_CV (0x%X, mbest_needcv): %s\n",
    MB_CV, (mb->flags & MB_CV) ? "on" : "off");

  /* MB_SYMWIN: use symmetrical window */
  fprintf(fp, "mb->flags & MB_SYMWIN (0x%X, mbest_sym_mbin): %s\n",
    MB_SYMWIN, (mb->flags & MB_SYMWIN) ? "on" : "off");

  /* MB_SINGLE_BIN: use single bin estimator */
  fprintf(fp, "mb->flags & MB_SINGLE_BIN (0x%X, mbest_single_bin): %s\n",
    MB_SINGLE_BIN, (mb->flags & MB_SINGLE_BIN) ? "on" : "off");

  /* MB_VERBOSE: being verbose */
  fprintf(fp, "mb->flags & MB_VERBOSE (0x%X, mbest_verbose): %s\n",
    MB_VERBOSE, (mb->flags & MB_VERBOSE) ? "on" : "off");

  at_mb_win__manifest(mb->win, fp, arrmax);

  /* nstrefresh: interval of recalculating et for all temperature */
  fprintf(fp, "mb->nstrefresh: int, %4d\n", mb->nstrefresh);

  /* nst_save_av: interval of writing mbav and ze files */
  fprintf(fp, "mb->nst_save_av: int, %4d\n", mb->nst_save_av);

  /* av_binary: use binary format in mbav file */
  fprintf(fp, "mb->av_binary: int, %4d\n", mb->av_binary);

  /* av_file: name of mbav file */
  fprintf(fp, "mb->av_file: char *, %s\n", mb->av_file);

  /* ze_file: name of ze file */
  fprintf(fp, "mb->ze_file: char *, %s\n", mb->ze_file);

  /* wze_reps: number of iterations before writing ze file */
  fprintf(fp, "mb->wze_reps: int, %4d\n", mb->wze_reps);

  /* visits: number of visits */
  fprintf(fp, "mb->visits: dynamic array of mb->n: ");
  for (i = mb->n-1; i >= 0; i--) if (fabs(mb->visits[i]) > 1e-30) break;
  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && mb->n > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < mb->n; i++) {
      if (i == arrmax && i < mb->n-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (mb->n-arrmax)) continue;
      fprintf(fp, "%g, ", mb->visits[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

  /* total_visits: total number of visits, number of tempering */
  fprintf(fp, "mb->total_visits: double, %g\n", mb->total_visits);

  /* frac_min: minimal allowable coefficient during left/right combination */
  fprintf(fp, "mb->iie->lr->frac_min: double, %g\n", mb->iie->lr->frac_min);

  /* cv_shift_max: maximal fraction for shift energy fluct. if cv is monotonic, it should be 0.0, for ising model, it can restrain the magnitude */
  fprintf(fp, "mb->iie->lr->cv_shift_max: double, %g\n", mb->iie->lr->cv_shift_max);

  at_mb_betadist__manifest(mb->betadist, fp, arrmax);

  at_mb_shk__manifest(mb->shk, fp, arrmax);

  at_mb_iie__manifest(mb->iie, fp, arrmax);

  at_mb_accum__manifest(mb->accum, fp, arrmax);

  /* cnt_int: number of additional integer variables to be written to binary file */
  fprintf(fp, "mb->cnt_int: int, %4d\n", mb->cnt_int);

  /* cnt_dbl: number of additional double variables to be written to binary file */
  fprintf(fp, "mb->cnt_dbl: int, %4d\n", mb->cnt_dbl);
}

#endif
