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

#ifndef AT_UTILS_LOCKFILE__SRC_H__
#define AT_UTILS_LOCKFILE__SRC_H__

#include "at_utils_lockfile.h"

#include "../sys/at_utils_sys.h"


static char *at_utils_lockfile__get_file_name(
    at_utils_lockfile_t *lockfile,
    zcom_ssm_t *ssm,
    const char *data_dir)
{
  char *fn;

  if (data_dir != NULL) {

    at_utils_sys__create_dir_if_not_exists(data_dir);

    fn = zcom_ssm__dup(ssm, data_dir);

    // if the last character is not '/',
    // append one
    char *p = fn + strlen(fn) - 1;

    if (*p != '/') {
      zcom_ssm__concat(ssm, &fn, "/");
    }

  } else {

    fn = zcom_ssm__dup(ssm, "");

  }

  zcom_ssm__concat(ssm, &fn, "lockfile");

  return lockfile->filename = fn;
}



int at_utils_lockfile__init(
    at_utils_lockfile_t *lockfile,
    at_bool_t ignore_lockfile,
    at_utils_log_t *log,
    zcom_ssm_t *ssm,
    const char *data_dir,
    at_bool_t verbose)
{
  at_utils_lockfile__get_file_name(lockfile, ssm, data_dir);

  at_utils_log__push_mod(log, "at.utils.lockfile");

  if (at_utils_sys__file_exists(lockfile->filename)) {

    at_utils_log__exit_if (!ignore_lockfile,
        log,

        // inform the user that the lock file exists
        // another simulation may be running

        "lockfile [%s] already exists, "
        "this probably means that a previous simulation is still running. "
        "If this is not so, please delete the file and restart the simulation.\n",
        lockfile->filename);

  } else {

    FILE *fp;

    if ((fp = fopen(lockfile->filename, "w")) == NULL) {

      at_utils_log__warning(log, "failed to create the lockfile %s\n", lockfile->filename);

    } else {

      fprintf(fp, "%ld\n", (long) time(NULL));

      fprintf(fp, "\n");

      fclose(fp);

      if (verbose) {
        at_utils_log__info(log, "successfully created the lockfile %s\n", lockfile->filename);
      }

    }

  } 

  at_utils_log__pop_mod(log);

  lockfile->ready = AT__TRUE;

  return 0;
}



void at_utils_lockfile__finish(
    at_utils_lockfile_t *lockfile)
{

  if (lockfile->ready) {

    if (at_utils_sys__file_exists(lockfile->filename)) {

      // remove the lock file
      remove(lockfile->filename);

    }

  }

}


#endif
