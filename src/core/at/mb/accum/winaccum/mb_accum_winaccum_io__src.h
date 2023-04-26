#ifndef AT__MB_ACCUM_WINACCUM_IO__SRC_H__
#define AT__MB_ACCUM_WINACCUM_IO__SRC_H__


#include "mb_accum_winaccum.h"

#include "../../sm/mb_sm.h"
#include "../../../zcom/util/util.h"
#include "../../../zcom/cfg/cfg.h"
#include "../../../zcom/endn/endn.h"


void at_mb_accum_winaccum__manifest(const at_mb_accum_winaccum_t *winaccum, FILE *fp, int arrmax)
{
}



int at_mb_accum_winaccum__read_binary(at_mb_accum_winaccum_t *winaccum, FILE *fp, int endn)
{

  int i;

  for (i = 0; i < winaccum->n; i++) {

    if (at_mb_accum_winaccum_item__read_binary(winaccum->items+i, fp, endn) != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



int at_mb_accum_winaccum__write_binary(at_mb_accum_winaccum_t *winaccum, FILE *fp)
{
  int i;

  for (i = 0; i < winaccum->n; i++) {

    if (at_mb_accum_winaccum_item__write_binary(winaccum->items+i, fp) != 0) {
      goto ERR;
    }

  }

  return 0;

ERR:

  return -1;
}



#endif