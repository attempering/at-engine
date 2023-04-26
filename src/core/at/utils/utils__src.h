#ifndef AT__UTILS__SRC_H__
#define AT__UTILS__SRC_H__



#include "utils_dir__src.h"
#include "../zcom/ssm/ssm__src.h"



char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn)
{
  char *buf;

  if (data_dir != NULL) {
    at_utils__create_dir_if_not_exists(data_dir);

    /* buf = data_dir + "/" + fn */
    buf = zcom_ssm__dup(ssm, data_dir);
    zcom_ssm__concat(ssm, &buf, "/");
    zcom_ssm__concat(ssm, &buf, fn);

  } else {
    buf = zcom_ssm__dup(ssm, fn);
  }

  return buf;
}



#endif
