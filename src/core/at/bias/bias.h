#ifndef AT__BIAS_H__
#define AT__BIAS_H__

#include "bias__def.h"
#include "../../zcom/zcom.h"

void at_bias__cfg_init(at_bias_t *bias, zcom_cfg_t *cfg, int silent);

void at_bias__finish(at_bias_t *bias);

void at_bias__manifest(at_bias_t *bias, FILE *fp, int arrmax);


#endif
