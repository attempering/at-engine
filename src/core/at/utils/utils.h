#ifndef AT__UTILS_H__
#define AT__UTILS_H__

#include "utils_dir.h"
#include "../../zcom/zcom.h"

char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn);

#endif