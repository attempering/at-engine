#ifndef AT__LANGEVIN_ZEROFILLER_H__
#define AT__LANGEVIN_ZEROFILLER_H__


#include "../../mb/mb__def.h"


int at_langevin_zerofiller__init(at_langevin_zerofiller_t *zf, at_mb_t *mb);

void at_langevin_zerofiller__finish(at_langevin_zerofiller_t *zf);

double *at_langevin_zerofiller__fill_range_custom(at_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end,
    bin_value_get_func_t bin_value_get_func);

double *at_langevin_zerofiller__fill_range_with_proper_sums(at_langevin_zerofiller_t *zf,
    int ib_begin, int ib_end);


#endif
