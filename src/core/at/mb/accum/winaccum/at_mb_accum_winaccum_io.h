#ifndef AT_MB_ACCUM_WINACCUM_IO_H__
#define AT_MB_ACCUM_WINACCUM_IO_H__


#include "at_mb_accum_winaccum__def.h"
#include "../../at_mb__def.h"
#include "../../win/at_mb_win.h"
#include "../../shk/at_mb_shk.h"
#include <stdio.h>

void at_mb_accum_winaccum__manifest(const at_mb_accum_winaccum_t *winaccum, FILE *fp, int arrmax);

int at_mb_accum_winaccum__read_binary(at_mb_accum_winaccum_t *winaccum, FILE *fp, int endn);

int at_mb_accum_winaccum__write_binary(at_mb_accum_winaccum_t *winaccum, FILE *fp);


#endif