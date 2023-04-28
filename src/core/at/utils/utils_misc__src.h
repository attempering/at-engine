#ifndef AT__UTILS_MISC__SRC_H__
#define AT__UTILS_MISC__SRC_H__



#include "sys/utils_sys.h"
#include "utils_misc.h"



char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn)
{
  char *buf;

  if (data_dir != NULL) {
    at_utils_sys__create_dir_if_not_exists(data_dir);

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
