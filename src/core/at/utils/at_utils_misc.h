#ifndef UTILS_MISC_H__
#define UTILS_MISC_H__

#include "sys/at_utils_sys.h"
#include "../../zcom/zcom.h"

char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn);

#endif
