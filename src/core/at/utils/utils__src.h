#ifndef AT__UTILS__SRC_H__
#define AT__UTILS__SRC_H__



#include "utils_dir__src.h"
#include "../zcom/ssm/ssm__src.h"



char *utils_make_output_filename(ssm_t *ssm, const char *data_dir, const char *fn)
{
  char *buf;

  if (data_dir != NULL) {
    create_dir_if_not_exists(data_dir);

    /* buf = data_dir + "/" + fn */
    buf = ssm_dup(ssm, data_dir);
    ssm_concat(ssm, &buf, "/");
    ssm_concat(ssm, &buf, fn);

  } else {
    buf = ssm_dup(ssm, fn);
  }

  return buf;
}



#endif
