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

#ifndef AT_EH_BASIC__SRC_H__
#define AT_EH_BASIC__SRC_H__

#include "at_eh_basic.h"
#include "recon/at_eh_recon.h"
#include "../mb/win/at_mb_win.h"
#include "../distr/at_distr.h"

int at_eh__conf_init(at_eh_t *eh,
    at_mb_t *mb,
    at_utils_conf_t *conf)
{
  int i;
  at_distr_t *distr = mb->distr;
  at_distr_domain_t *domain = distr->domain;

  eh->mb = mb;
  eh->n = distr->domain->n;

  eh->e_min_runtime = 0;
  eh->e_max_runtime = 0;

  at_utils_conf__push_mod(conf, "at.eh");

  /* eh_mode: 0: disable; 1: simple histogram */
  at_utils_conf__get_int(conf,
      "ehist-mode",
      &eh->mode, 0,
      "mode");

  if ( !(eh->mode == 0 || eh->mode == 1) ) {
    fprintf(stderr, "eh->mode: failed validation: eh->mode == 0 || eh->mode == 1\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  if (eh->mode == 0) {
    return 0;
  }


  /* eh_min: minimal energy */
  at_utils_conf__get_double(conf,
      "ehist-min,ehist-e-min",
      &eh->e_min, -12.6e4,
      "e_min");

  eh->e_min_runtime = eh->e_min;

  /* eh_max: maximal energy */
  at_utils_conf__get_double(conf,
      "ehist-max,ehist-e-max",
      &eh->e_max, -9.0e4,
      "e_max");

  eh->e_max_runtime = eh->e_max;

  /* eh_del: energy bin size */
  at_utils_conf__get_double(conf,
      "ehist_del,ehist-e-del",
      &eh->e_del, 20.0,
      "e_del");

  if (eh->e_del <= 0) {
    at_utils_log__error(conf->log,
        "eh->e_del: failed validation: eh->e_del > 0\n");
    goto ERR;
  }

  /* eh_cnt: number of energy bins */
  eh->e_n = (int)((eh->e_max-eh->e_min)/eh->e_del - 1e-5 + 1);

  at_utils_conf__get_bool(conf,
      "ehist-use-text-file",
      &eh->use_text_file, AT__FALSE,
      "use_text_file");

  /* eh_nst_save: interval of writing histogrm files */
  at_utils_conf__get_double(conf,
      "nsthist,nst-hist,ehist-nst-save",
      &eh->nst_save, 100000,
      "nst_save");


  at_utils_conf__get_filename(conf,
      "ehist-file,ehist-file-binary",
      &eh->file_binary, "hist.dat",
      "file_binary");

  at_utils_conf__get_filename(conf,
      "ehist-file-text",
      &eh->file_text, "hist-text.dat",
      "file_text");

  /* eh_his: energy histogram data */
  if ((eh->his = (double *) calloc((eh->n*eh->e_n + 1), sizeof(double))) == NULL) {
    fprintf(stderr, "Error@at.eh: no memory! var: eh->his, type: double\n");
    fprintf(stderr, "    src: %s:%d\n", __FILE__, __LINE__);
    exit(1);
  }
  for (i = 0; i < eh->n*eh->e_n; i++) {
    eh->his[i] = 0.0;
  }

  /* add a half energy bin width in output */
  at_utils_conf__get_bool(conf,
      "ehist-addahalf,ehist-add-half-ebin",
      &eh->add_half_ebin, AT__FALSE,
      "add_half_ebin");

  /* keep zero margins */
  at_utils_conf__get_bool(conf,
      "ehist-keepedge,ehist-keep-margin,ehist-keep-margins",
      &eh->keep_margins, AT__FALSE,
      "keep_margins");

  /* do not output zeroes */
  at_utils_conf__get_bool(conf,
      "ehist-nozeroes,ehist-no-zeroes,ehist-no-zeros",
      &eh->no_zeros, AT__FALSE,
      "no_zeros");

  at_utils_conf__pop_mod(conf);

  at_eh_recon__conf_init(eh->recon, eh, conf);

  return 0;

ERR:
  return -1;
}


void at_eh__clear(at_eh_t *eh)
{
  int i;

  if (eh->mode <= 0) {
    return;
  }

  for (i = 0; i < eh->n*eh->e_n; i++) {
    eh->his[i] = 0.0;
  }

  at_eh_recon__clear(eh->recon);

}


void at_eh__finish(at_eh_t *eh)
{
  at_eh_recon__finish(eh->recon);
  if (eh->his != NULL) {
    free(eh->his);
    eh->his;
  }
}


void at_eh__manifest(const at_eh_t* eh, at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.eh");

  /* eh_mode: 0: disable; 1: simple histogram */
  at_utils_manifest__print_int(manifest, eh->mode, "mode", "ehist-mode");

  if (eh->mode == 0) {
    return;
  }

  /* eh_min: minimal energy */
  at_utils_manifest__print_double(manifest, eh->e_min, "e_min", "ehist-e-min");

  /* eh_max: maximal energy */
  at_utils_manifest__print_double(manifest, eh->e_max, "e_max", "ehist-e-max");

  /* eh_del: energy bin size */
  at_utils_manifest__print_double(manifest, eh->e_del, "e_del", "ehist-e-del");

  /* eh_cnt: number of energy bins */
  at_utils_manifest__print_int(manifest, eh->e_n, "e_n", NULL);

  at_utils_manifest__print_bool(manifest, eh->use_text_file, "use_text_file", "ehist-use-text-file");

  /* eh_nst_save: interval of writing histogram files */
  at_utils_manifest__print_int(manifest, eh->nst_save, "nst_save", "ehist-nst-save");

  /* name of ehist files */
  at_utils_manifest__print_str(manifest, eh->file_binary, "file_binary", "ehist-file-binary");
  at_utils_manifest__print_str(manifest, eh->file_text, "file_text", "ehist-file-text");

  /* eh_his: energy histogram data */
  at_utils_manifest__print_double_arr(manifest, eh->his, eh->n*eh->e_n, "his");

  /* whether to add a half energy bin width in output */
  at_utils_manifest__print_bool(manifest, eh->add_half_ebin, "add_half_ebin", "ehist-add-half-ebin");

  /* whether to keep empty margin */
  at_utils_manifest__print_bool(manifest, eh->keep_margins, "keep_margins", "ehist-keep-margins");

  /* whether to include empty energy bins in the output */
  at_utils_manifest__print_bool(manifest, eh->no_zeros, "no_zeros", "ehist-no-zeros");

  at_utils_manifest__pop_mod(manifest);

  at_eh_recon__manifest(eh->recon, manifest);
}



#endif
