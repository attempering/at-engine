#ifndef AT__MB_ZEROFILLER_H__
#define AT__MB_ZEROFILLER_H__


#include "../mb__def.h"


int mb_zerofiller__init(mb_zerofiller_t *zf, mb_t *mb);

void mb_zerofiller__finish(mb_zerofiller_t *zf);

void mb_zerofiller_fill_range_custom(mb_zerofiller_t *zf, int ib_begin, int ib_end,
    bin_value_get_func_t bin_value_get_func);

void mb_zerofiller_fill_range_with_proper_sums(mb_zerofiller_t *zf, int ib_begin, int ib_end);


#endif
