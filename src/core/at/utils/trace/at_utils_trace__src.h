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

#ifndef AT_UTILS_TRACE__SRC_H__
#define AT_UTILS_TRACE__SRC_H__

#include "at_utils_trace.h"

#include "../../../zcom/zcom.h"
#include "../at_utils_misc.h"
#include "../manifest/at_utils_manifest.h"



void at_utils_trace__cfg_init(at_utils_trace_t *trace,
    zcom_cfg_t *cfg,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  trace->filename = (char *) "trace.dat";
  if (zcom_cfg__get(cfg, &trace->filename, "trace-file", "%s") != 0)
  {
    if (verbose) fprintf(stderr, "Info@at.utils.trace: assuming default utils->trace->filename = \"trace.dat\", key: trace-file\n");
  }

  trace->filename = at_utils__make_output_filename(ssm, data_dir, trace->filename);

  /* nst_trace: interval of writing trace file; -1: only when doing neighbor search, 0: disable */
  trace->nst_trace = -1;
  if (zcom_cfg__get(cfg, &trace->nst_trace, "nsttrace,nst-trace", "%d")) {
    if (verbose) fprintf(stderr, "Info@at.utils.trace: assuming default utils->trace->nst_trace = -1, key: nst-trace\n");
  }

  // do not open trace file yet, at__open_log();
  trace->log = NULL;

  trace->inited = 1;
}



void at_utils_trace__finish(at_utils_trace_t *trace)
{
  if (trace->inited) {
    at_utils_trace__close_file(trace);
  }
}



void at_utils_trace__manifest(at_utils_trace_t *trace, at_utils_manifest_t *manifest)
{
  fprintf(manifest->fp, "utils->trace->nst_trace: int, %4d\n", trace->nst_trace);

  fprintf(manifest->fp, "utils->trace->filename: char *, %s\n", trace->filename);
}




zcom_log_t *at_utils_trace__open_file(at_utils_trace_t *trace, at_bool_t is_continuation)
{
  trace->log = zcom_log__open(trace->filename,
      (is_continuation ? ZCOM_LOG__APPEND : 0));

  return trace->log;
}


void at_utils_trace__close_file(at_utils_trace_t *trace)
{
  if (trace->log) {
    zcom_log__close(trace->log);
    trace->log = NULL;
  }
}



// decide whether to write trace at this step
at_bool_t at_utils_trace__decide_do_trace(at_utils_trace_t *trace,
    const at_params_step_t *step_params)
{
  int nst_trace = trace->nst_trace;
  at_bool_t do_trace;

  // whether to write the trace file
  if (nst_trace > 0) {

    do_trace = (step_params->step % nst_trace == 0)
           || step_params->is_first_step
           || step_params->is_last_step;

  } else if (nst_trace < 0) {
    // logging is disabled if at->nst_trace < 0
    do_trace = AT__FALSE;
  } else {
    // do it according to the variable do_trace
    do_trace = step_params->do_trace;
  }

  return do_trace;
}



#endif
