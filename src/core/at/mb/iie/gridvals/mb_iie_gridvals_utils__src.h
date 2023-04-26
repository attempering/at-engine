#ifndef AT__MB_IIE_GRIDVALS_UTILS__SRC_H__
#define AT__MB_IIE_GRIDVALS_UTILS__SRC_H__



/* integral-identity estimators for grid point values */



#include "mb_iie_gridvals_utils.h"




/* set quality bit */
void mb_iie_gridvals_item__set_quality_bit(
    mb_iie_gridvals_item_t *item,
    unsigned mask, int on)
{
  if (mask) {
    if (on) {
      item->quality_bits |= mask;
    } else {
      item->quality_bits &= ~mask;
    }
  }
}


/* translate the quality bits into an array of 0-1 characters */
char *mb_iie_gridvals_item__quality_bits_to_string(const mb_iie_gridvals_item_t *item)
{
  static char buf[64]; /* has to be static to be the return value */
  int  cnt = 0;

  buf[cnt++] = '1';
  buf[cnt++] = (char)((item->quality_bits & MB_IIE_GRIDVALS__QUALITY_BIT_EHAT) ? '1' : '0');
  buf[cnt++] = (char)((item->quality_bits & MB_IIE_GRIDVALS__QUALITY_BIT_CV  ) ? '1' : '0');
  buf[cnt++] = (char)((item->quality_bits & MB_IIE_GRIDVALS__QUALITY_BIT_LNZ ) ? '1' : '0');
  buf[cnt] = '\0';

  return buf;
}




#endif
