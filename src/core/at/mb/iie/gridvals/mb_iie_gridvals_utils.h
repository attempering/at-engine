#ifndef AT__MB_IIE_GRIDVALS_UTILS_H__
#define AT__MB_IIE_GRIDVALS_UTILS_H__


#include "mb_iie_gridvals__def.h"

typedef struct at_mb_t_ at_mb_t;
typedef struct zcom_cfg_t_ zcom_cfg_t;


void at_mb_iie_gridvals_item__set_quality_bit(
    at_mb_iie_gridvals_item_t *item,
    unsigned mask, int on);

char *at_mb_iie_gridvals_item__quality_bits_to_string(
    const at_mb_iie_gridvals_item_t *item);

#endif
