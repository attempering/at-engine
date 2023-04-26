#ifndef AT__MB_ZEROFILLER_H__
#define AT__MB_ZEROFILLER_H__


#include "../mb__def.h"


int at_mb_zerofiller__init(at_mb_zerofiller_t *zf, at_mb_t *mb);

void at_mb_zerofiller__finish(at_mb_zerofiller_t *zf);

void mb_zerofiller_fill_range_custom(at_mb_zerofiller_t *zf, int ib_begin, int ib_end,
    bin_value_get_func_t bin_value_get_func);

void mb_zerofiller_fill_range_with_proper_sums(at_mb_zerofiller_t *zf, int ib_begin, int ib_end);


#endif
