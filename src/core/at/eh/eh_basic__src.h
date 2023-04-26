#ifndef AT__EH_BASIC__SRC_H__
#define AT__EH_BASIC__SRC_H__

#include "eh_basic.h"
#include "../mb/win/mb_win.h"

#include "../zcom/cfg/cfg.h"


int eh__cfg_init(eh_t *eh, cfg_t *cfg, mb_t *mb, const char *data_dir)
{
  int i;

  eh->mb = mb;
  eh->n = mb->n;

  /* eh_mode: 0: disable; 1: simple histogram */
  eh->mode = 0;
  if (0 != cfg_get(cfg, &eh->mode, "ehist_mode", "%d")) {
    fprintf(stderr, "assuming default: eh->mode = 0, key: ehist_mode\n");
  }
  if ( !(eh->mode == 0 || eh->mode == 1) ) {
    fprintf(stderr, "eh->mode: failed validation: eh->mode == 0 || eh->mode == 1\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* eh_skip: interval of reconstructing energy histograms */
  eh->skip = 10;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->skip, "ehist_skip", "%d")) {
      fprintf(stderr, "assuming default: eh->skip = 10, key: ehist_skip\n");
    }
    if ( !(eh->skip > 0) ) {
      fprintf(stderr, "eh->skip: failed validation: eh->skip > 0\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwmod: 0: d(beta) 1: dT/T  2: d(kT) */
  eh->bwmod = 1;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->bwmod, "ehist_mbin_mode", "%d")) {
      fprintf(stderr, "assuming default: eh->bwmod = 1, key: ehist_mbin_mode\n");
    }
    if ( !(eh->bwmod >= 0 && eh->bwmod <= 2) ) {
      fprintf(stderr, "eh->bwmod: failed validation: eh->bwmod >= 0 && eh->bwmod <= 2\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta lnT */
  eh->bwdel = 0.05;
  if (eh->mode && eh->bwmod == 1) {
    if (cfg != NULL && 0 != cfg_get(cfg, &eh->bwdel, "ehist_delta_lnT", "%lf")) {
      fprintf(stderr, "missing var: eh->bwdel, key: ehist_delta_lnT, fmt: %%lf\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if ( !(eh->bwdel > mb->bdel/pow(mb->bmin, 1.0)) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel > mb->bdel/pow(mb->bmin, 1.0)\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta beta */
  if (eh->mode && eh->bwmod == 0) {
    eh->bwdel = 0.02;
    if (cfg != NULL && 0 != cfg_get(cfg, &eh->bwdel, "ehist_delta_beta", "%lf")) {
      fprintf(stderr, "missing var: eh->bwdel, key: ehist_delta_beta, fmt: %%lf\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if ( !(eh->bwdel > mb->bdel/pow(mb->bmin, 0.0)) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel > mb->bdel/pow(mb->bmin, 0.0)\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_bwdel: delta kT */
  if (eh->mode && eh->bwmod == 2) {
    eh->bwdel = 0.10;
    if (cfg != NULL && 0 != cfg_get(cfg, &eh->bwdel, "ehist_delta_kT", "%lf")) {
      fprintf(stderr, "missing var: eh->bwdel, key: ehist_delta_kT, fmt: %%lf\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if ( !(eh->bwdel > mb->bdel/pow(mb->bmin, 2.0)) ) {
      fprintf(stderr, "eh->bwdel: failed validation: eh->bwdel > mb->bdel/pow(mb->bmin, 2.0)\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_min: minimal energy */
  eh->min = -12.6e4;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->min, "ehist_min", "%lf")) {
      fprintf(stderr, "assuming default: eh->min = -12.6e4, key: ehist_min\n");
    }
  }

  /* eh_max: maximal energy */
  eh->max = -9.0e4;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->max, "ehist_max", "%lf")) {
      fprintf(stderr, "assuming default: eh->max = -9.0e4, key: ehist_max\n");
    }
    if ( !(eh->max > eh->min) ) {
      fprintf(stderr, "eh->max: failed validation: eh->max > eh->min\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_del: energy bin size */
  eh->del = 20.0;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->del, "ehist_del", "%lf")) {
      fprintf(stderr, "assuming default: eh->del = 20.0, key: ehist_del\n");
    }
    if ( !(eh->del > 0) ) {
      fprintf(stderr, "eh->del: failed validation: eh->del > 0\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* eh_cnt: number of energy bins */
  eh->cnt = (int)((eh->max-eh->min)/eh->del - 1e-5 + 1);
  /* eh_binary: binary format for ehist file */
  eh->binary = 1;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->binary, "ehist_binary", "%d")) {
      fprintf(stderr, "assuming default: eh->binary = 1, key: ehist_binary\n");
    }
  }

  /* eh_nst_save: interval of writing histogrm files */
  eh->nst_save = 100000;
  if (eh->mode) {
    if (0 != cfg_get(cfg, &eh->nst_save, "nsthist", "%d")) {
      fprintf(stderr, "assuming default: eh->nst_save = 100000, key: nsthist\n");
    }
  }

  /* eh_file: name of ehist file */
  eh->file = NULL;
  if (eh->mode) {
    char *fn_eh = "hist.dat";
    if (0 != cfg_get(cfg, &fn_eh, "ehist_file", "%s")) {
      fprintf(stderr, "assuming default: eh->file = \"%s\", key: ehist_file\n", fn_eh);
    }
    eh->file = utils_make_output_filename(mb->ssm, data_dir, fn_eh);
  }

  /* eh_rfile: name of reconstructed energy histogram */
  eh->rfile = NULL;
  if (eh->mode) {
    char *fn_eh_mb = "hist_mb.dat";
    if (0 != cfg_get(cfg, &fn_eh_mb, "ehist_mbin_file", "%s")) {
      fprintf(stderr, "assuming default: eh->rfile = \"%s\", key: ehist_mbin_file\n", fn_eh_mb);
    }
    eh->rfile = utils_make_output_filename(mb->ssm, data_dir, fn_eh_mb);
  }

  /* eh_his: energy histogram data */
  eh->his = NULL;
  if (eh->mode) {
    if ((eh->his = (double *) calloc((eh->n*eh->cnt + 1), sizeof(double))) == NULL) {
      fprintf(stderr, "no memory! var: eh->his, type: double\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n*eh->cnt; i++)
      eh->his[i] = 0.0;
  }
  /* eh_recon: temporary space for reconstructing histogram */
  eh->recon = NULL;
  if (eh->mode) {
    if ((eh->recon = (double *) calloc((eh->cnt + 1), sizeof(double))) == NULL) {
      fprintf(stderr, "no memory! var: eh->recon, type: double\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->cnt; i++)
      eh->recon[i] = 0.0;
  }
  /* eh_is: indices for temperature windows (lower) */
  eh->is = NULL;
  if (eh->mode) {
    if ((eh->is = (int *) calloc((eh->n + 2), sizeof(int))) == NULL) {
      fprintf(stderr, "no memory! var: eh->is, type: int\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n + 1; i++) {
      eh->is[i] = 0;
    }
  }

  /* eh_it: indices for temperature windows (higher) */
  eh->it = NULL;
  if (eh->mode) {
    if ((eh->it = (int *) calloc((eh->n + 2), sizeof(int))) == NULL) {
      fprintf(stderr, "no memory! var: eh->it, type: int\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      exit(1);
    }
    for (i = 0; i < eh->n + 1; i++) {
      eh->it[i] = 0;
    }

    fprintf(stderr, "eh->is %p, eh->it %p\n", eh->is, eh->it);

    // windows for the reconstructed energy histograms
    mb_win__make_unres_windows_for_grid_estimators(mb->n, mb->barr, mb->bdel,
        eh->bwmod, eh->bwdel, eh->is, eh->it);
  }

  /* EH_ADDAHALF: add a half energy bin width in output */
  if (eh->mode) {
    unsigned i = 1;
    if (0 != cfg_get(cfg, &i, "ehist_addahalf", "%u")) {
      fprintf(stderr, "assuming default: EH_ADDAHALF = 1, key: ehist_addahalf\n");
    }
    if ( !(i == 0 || i == 1) ) {
      fprintf(stderr, "EH_ADDAHALF: failed validation: i == 0 || i == 1\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if (i) {
      eh->flags |= EH_ADDAHALF;
    } else {
      eh->flags &= ~EH_ADDAHALF;
    }
  }

  /* EH_KEEPEDGE: keep zero edge at sides */
  if (eh->mode) {
    unsigned i = 0;
    if (0 != cfg_get(cfg, &i, "ehist_keepedge", "%u")) {
      fprintf(stderr, "assuming default: EH_KEEPEDGE = 0, key: ehist_keepedge\n");
    }
    if ( !(i == 0 || i == 1) ) {
      fprintf(stderr, "EH_KEEPEDGE: failed validation: i == 0 || i == 1\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if (i) {
      eh->flags |= EH_KEEPEDGE;
    } else {
      eh->flags &= ~EH_KEEPEDGE;
    }
  }

  /* EH_NOZEROES: do not output zeroes */
  if (eh->mode) {
    unsigned i = 0;
    if (0 != cfg_get(cfg, &i, "ehist_nozeroes", "%u")) {
      fprintf(stderr, "assuming default: EH_NOZEROES = 0, key: ehist_nozeroes\n");
    }
    if ( !(i == 0 || i == 1) ) {
      fprintf(stderr, "EH_NOZEROES: failed validation: i == 0 || i == 1\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if (i) {
      eh->flags |= EH_NOZEROES;
    } else {
      eh->flags &= ~EH_NOZEROES;
    }
  }

  return 0;

ERR:
  return -1;
}


void eh__clear(eh_t *eh)
{
  int i;

  if (eh->mode <= 0) {
    return;
  }

  if (eh->mode) {
    for (i = 0; i < eh->n*eh->cnt; i++) {
      eh->his[i] = 0.0;
    }

    for (i = 0; i < eh->cnt; i++) {
      eh->recon[i] = 0.0;
    }
  }

}


void eh__finish(eh_t *eh)
{
  /* when eh->mode == 0, data members are not allocated */
  if (eh->mode) {
    if (eh->his    != NULL) free(eh->his);
    if (eh->recon  != NULL) free(eh->recon);
    if (eh->is     != NULL) free(eh->is);
    if (eh->it     != NULL) free(eh->it);
  }
}

#endif
