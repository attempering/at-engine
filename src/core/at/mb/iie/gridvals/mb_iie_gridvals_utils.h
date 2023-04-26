#ifndef AT__MB_IIE_GRIDVALS_UTILS_H__
#define AT__MB_IIE_GRIDVALS_UTILS_H__


#include "mb_iie_gridvals__def.h"

typedef struct mb_t_ mb_t;
typedef struct cfg_t_ cfg_t;


void mb_iie_gridvals_item__set_quality_bit(
    mb_iie_gridvals_item_t *item,
    unsigned mask, int on);

char *mb_iie_gridvals_item__quality_bits_to_string(const mb_iie_gridvals_item_t *item);

#endif
