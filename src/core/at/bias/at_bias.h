#ifndef BIAS_H__
#define BIAS_H__

#include "at_bias__def.h"
#include "../../zcom/zcom.h"

void at_bias__cfg_init(at_bias_t *bias, zcom_cfg_t *cfg, int silent);

void at_bias__finish(at_bias_t *bias);

void at_bias__manifest(at_bias_t *bias, FILE *fp, int arrmax);


#endif
