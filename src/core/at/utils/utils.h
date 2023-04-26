#ifndef AT__UTILS_H__
#define AT__UTILS_H__

#include "utils_dir.h"
#include "../zcom/ssm/ssm.h"

char *utils_make_output_filename(ssm_t *ssm, const char *data_dir, const char *fn);

#endif