#ifndef AT__MB_IIE_ZEROFILLER__H__
#define AT__MB_IIE_ZEROFILLER__H__


#include "mb_iie_zerofiller__def.h"


void at_mb_iie_zerofiller__init(at_mb_iie_zerofiller_t *zf, at_mb_t *mb);

void at_mb_iie_zerofiller__finish(at_mb_iie_zerofiller_t *zf);

/**
 * Given zf->vals[] and zf->has_vals[],
 * fill the values of empty bins in the range [ib_begin, ib_end].
 * Empty bins are marked by zf->has_vals[].
 * The filled values are written into zf->vals[].
 *
 * The algorithm we used here is a two-pass left/right scan algorithm.
 * 
 * return the number unfilled bins.
 * 
 * The return value is 0, if there is a single data point
 * in the specified range, or the number of bins in the range
 * on an empty range.
 */
int at_mb_iie_zerofiller__fill_missing(at_mb_iie_zerofiller_t *zf, int ib_begin, int ib_end);

#endif
