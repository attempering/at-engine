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

#ifndef AT_EH_IO__SRC_H__
#define AT_EH_IO__SRC_H__

#include "at_eh_io.h"
#include "at_eh_basic.h"

/* include the source code of sub-modules */
#include "at_eh_io_binary__src.h"

#include "../../zcom/zcom.h"

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



void at_eh__manifest(const at_eh_t* eh, at_utils_manifest_t *manifest)
{
  FILE *fp = manifest->fp;

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
  at_utils_manifest__print_double_arr(manifest, eh->his, eh->n*eh->cnt, "eh->his");

  /* eh_recon: temporary space for reconstructing histogram */
  at_utils_manifest__print_double_arr(manifest, eh->recon, eh->cnt, "eh->recon");

  /* eh_is: indices for temperature windows (lower) */
  at_utils_manifest__print_int_arr(manifest, eh->is, eh->n + 1, "eh->is");

  /* eh_it: indices for temperature windows (higher) */
  at_utils_manifest__print_int_arr(manifest, eh->it, eh->n + 1, "eh->it");

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
