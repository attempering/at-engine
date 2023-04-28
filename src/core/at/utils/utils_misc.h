#ifndef AT__UTILS_MISC_H__
#define AT__UTILS_MISC_H__

#include "sys/utils_sys.h"
#include "../../zcom/zcom.h"

char *at_utils__make_output_filename(zcom_ssm_t *ssm, const char *data_dir, const char *fn);

#endif
