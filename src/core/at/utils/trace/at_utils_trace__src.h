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
#include "../at_utils_misc.h"
#include "../manifest/at_utils_manifest.h"



void at_utils_trace__conf_init(at_utils_trace_t *trace,
    at_utils_conf_t *conf)
{
  at_utils_conf__push_mod(conf, "at.utils.trace");

  at_utils_conf__get_filename(conf,
      "trace-file",
      &trace->file, "trace.dat",
      "file");

  /* nst_trace: interval of writing trace file; -1: only when doing neighbor search, 0: disable */
  at_utils_conf__get_int(conf,
      "nsttrace,nst-trace",
      &trace->nst_trace, -1,
      "nst_trace");

  at_utils_conf__pop_mod(conf);

  // do not open trace file yet, at__open_log();
  trace->logger = NULL;

  trace->ready = AT__TRUE;
}



void at_utils_trace__finish(at_utils_trace_t *trace)
{
  if (trace->ready) {
    at_utils_trace__close_file(trace);
  }
}



void at_utils_trace__manifest(
    const at_utils_trace_t *trace,
    at_utils_manifest_t *manifest)
{
  at_utils_manifest__push_mod(manifest, "at.utils.trace");

  at_utils_manifest__print_int(manifest, trace->nst_trace, "nst_trace", "nst-trace");

  at_utils_manifest__print_str(manifest, trace->file, "file", "trace-file");

  at_utils_manifest__pop_mod(manifest);
}




zcom_logger_t *at_utils_trace__open_file(at_utils_trace_t *trace, at_bool_t is_continuation)
{
  trace->logger = zcom_logger__open(trace->file,
      (is_continuation ? ZCOM_LOGGER__APPEND : 0));

  return trace->logger;
}



void at_utils_trace__close_file(at_utils_trace_t *trace)
{
  if (trace->logger) {
    zcom_logger__close(trace->logger);
    trace->logger = NULL;
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
           || step_params->is_final_step;

  } else if (nst_trace < 0) {
    // logging is disabled if at->nst_trace < 0
    do_trace = AT__FALSE;
  } else {
    // do it according to the variable do_trace
    do_trace = step_params->do_trace;
  }

  return do_trace;
}



void at_utils_trace__printf(
    at_utils_trace_t *trace,
    at_bool_t flush_afterward,
    const char *fmt, ...)
{
  va_list args;

  if (trace->logger == NULL) {
    trace->logger = at_utils_trace__open_file(trace, AT__FALSE);
  }

  va_start(args, fmt);
  zcom_logger__vprintf(trace->logger, fmt, args);
  va_end(args);

  if (flush_afterward) {
    zcom_logger__flush(trace->logger);
  }
}



#endif
