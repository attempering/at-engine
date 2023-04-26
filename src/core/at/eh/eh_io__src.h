#ifndef AT__EH_IO__SRC_H__
#define AT__EH_IO__SRC_H__

#include "eh_io.h"
#include "eh_basic.h"

/* include the source code of sub-modules */
#include "eh_io_binary__src.h"

#include "../zcom/cfg/cfg.h"

int at_eh__read(at_eh_t *eh)
{
  int ret = 0, version;

  if (eh != NULL && eh->mode != 0) {
    ret = at_eh__read_binary(eh, eh->file, &version);
  }

  return ret;
}



int at_eh__write(at_eh_t *eh)
{
  int ret = 0;
  const int version = 0;

  if (eh != NULL && eh->mode != 0) {
    ret = at_eh__write_binary(eh, eh->file, version);
  }

  return ret;
}



void at_eh__manifest(const at_eh_t* eh, FILE *fp, int arrmax)
{
  int i;
  int pacnt;

  /* eh_mode: 0: disable; 1: simple histogram */
  fprintf(fp, "eh->mode: int, %4d\n", eh->mode);

  if (eh->mode == 0) {
    return;
  }

  /* eh_skip: interval of reconstructing energy histograms */
  fprintf(fp, "eh->skip: int, %4d\n", eh->skip);

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  fprintf(fp, "eh->bwmod: int, %4d\n", eh->bwmod);

  /* eh_bwdel: delta lnT */
  fprintf(fp, "eh->bwdel: double, %g\n", eh->bwdel);

  /* eh_min: minimal energy */
  fprintf(fp, "eh->min: double, %g\n", eh->min);

  /* eh_max: maximal energy */
  fprintf(fp, "eh->max: double, %g\n", eh->max);

  /* eh_del: energy bin size */
  fprintf(fp, "eh->del: double, %g\n", eh->del);

  /* eh_cnt: number of energy bins */
  fprintf(fp, "eh->cnt: int, %4d\n", eh->cnt);

  /* eh_binary: binary format for ehist file */
  fprintf(fp, "eh->binary: int, %4d\n", eh->binary);

  /* eh_nst_save: interval of writing histogrm files */
  fprintf(fp, "eh->nst_save: int, %4d\n", eh->nst_save);

  /* eh_file: name of ehist file */
  fprintf(fp, "eh->file: char *, %s\n", eh->file);

  /* eh_rfile: name of reconstructed energy histogram */
  fprintf(fp, "eh->rfile: char *, %s\n", eh->rfile);

  /* eh_his: energy histogram data */
  fprintf(fp, "eh->his: dynamic array of eh->n*eh->cnt: ");

  for (i = eh->n*eh->cnt-1; i >= 0; i--) {
    if (fabs(eh->his[i]) > 1e-30) {
      break;
    }
  }

  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && eh->n*eh->cnt > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < eh->n*eh->cnt; i++) {
      if (i == arrmax && i < eh->n*eh->cnt-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (eh->n*eh->cnt-arrmax)) continue;
      fprintf(fp, "%g, ", eh->his[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

  /* eh_recon: temporary space for reconstructing histogram */
  fprintf(fp, "eh->recon: dynamic array of eh->cnt: ");
  for (i = eh->cnt-1; i >= 0; i--) {
    if (fabs(eh->recon[i]) > 1e-30) {
      break;
    }
  }

  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && eh->cnt > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < eh->cnt; i++) {
      if (i == arrmax && i < eh->cnt-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (eh->cnt-arrmax)) continue;
      fprintf(fp, "%g, ", eh->recon[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

  /* eh_is: indices for temperature windows (lower) */
  fprintf(fp, "eh->is: dynamic array of eh->n + 1: ");
  for (i = eh->n + 1-1; i >= 0; i--) if (eh->is[i]) break;
  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && eh->n + 1 > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < eh->n + 1; i++) {
      if (i == arrmax && i < eh->n + 1-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (eh->n + 1-arrmax)) continue;
      fprintf(fp, "%4d, ", eh->is[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

  /* eh_it: indices for temperature windows (higher) */
  fprintf(fp, "eh->it: dynamic array of eh->n + 1: ");
  for (i = eh->n + 1-1; i >= 0; i--) if (eh->it[i]) break;
  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && eh->n + 1 > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < eh->n + 1; i++) {
      if (i == arrmax && i < eh->n + 1-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (eh->n + 1-arrmax)) continue;
      fprintf(fp, "%4d, ", eh->it[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

  /* AT_EH_ADDAHALF: add a half energy bin width in output */
  fprintf(fp, "eh->flags & AT_EH_ADDAHALF (0x%X, ehist_addahalf): %s\n",
    AT_EH_ADDAHALF, (eh->flags & AT_EH_ADDAHALF) ? "on" : "off");

  /* AT_EH_KEEPEDGE: keep zero edge at sides */
  fprintf(fp, "eh->flags & AT_EH_KEEPEDGE (0x%X, ehist_keepedge): %s\n",
    AT_EH_KEEPEDGE, (eh->flags & AT_EH_KEEPEDGE) ? "on" : "off");

  /* AT_EH_NOZEROES: do not output zeroes */
  fprintf(fp, "eh->flags & AT_EH_NOZEROES (0x%X, ehist_nozeroes): %s\n",
    AT_EH_NOZEROES, (eh->flags & AT_EH_NOZEROES) ? "on" : "off");
}

#endif
