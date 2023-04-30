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

#ifndef AT_UTILS_LOG__SRC_H__
#define AT_UTILS_LOG__SRC_H__

#include "at_utils_log.h"

#include "../../../zcom/zcom.h"
#include "../at_utils_misc.h"
#include "../manifest/at_utils_manifest.h"



void at_utils_log__cfg_init(at_utils_log_t *log,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  log->filename = "log.dat";
  if (zcom_cfg__get(cfg, &log->filename, "log_file", "%s") != 0)
  {
    if (verbose) fprintf(stderr, "Info: assuming default utils->log->filename = \"log.dat\", key: log_file\n");
  }

  log->filename = at_utils__make_output_filename(ssm, data_dir, log->filename);

  /* nst_log: interval of writing trace file; -1: only when doing neighbor search, 0: disable */
  log->nst_log = -1;
  if (zcom_cfg__get(cfg, &log->nst_log, "nsttrace", "%d")) {
    if (verbose) fprintf(stderr, "Info: assuming default utils->log->nst_log = -1, key: nsttrace\n");
  }
  
  {
    char *fn_log = "log.dat";

    /* log_file: name of trace file */
    if (zcom_cfg__get(cfg, &fn_log, "log_file", "%s"))
    {
      if (verbose) fprintf(stderr, "Info: assuming default utils->log->log_file = \"%s\", key: log_file\n", fn_log);
    }

    //fprintf(stderr, "log file %s => %s\n", fn_log, at->log_file);getchar();
  }

  // do not open log file yet, at__open_log();
  log->log = NULL;

  log->inited = 1;
}



void at_utils_log__finish(at_utils_log_t *log)
{
  if (log->inited) {
    at_utils_log__close_file(log);
  }
}


void at_utils_log__open_file(at_utils_log_t *log)
{
  log->log = zcom_log__open(log->filename);
}

void at_utils_log__close_file(at_utils_log_t *log)
{
  if (log->log) {
    zcom_log__close(log->log);
  }
}

void at_utils_log__manifest(at_utils_log_t *log, at_utils_manifest_t *manifest)
{
  fprintf(manifest->fp, "utils->log->nst_log: int, %4d\n", log->nst_log);

  fprintf(manifest->fp, "utils->log->fname: char *, %s\n", log->filename);
}

#endif
