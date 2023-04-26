#ifndef AT__MB_ACCUM_IO_H__
#define AT__MB_ACCUM_IO_H__


#include "mb_accum__def.h"
#include "../mb__def.h"
#include "../win/mb_win.h"
#include "../shk/mb_shk.h"
#include <stdio.h>

void mb_accum__manifest(const mb_accum_t *accum, FILE *fp, int arrmax);

int mb_accum__read_binary(mb_accum_t *accum, FILE *fp, int endn);

int mb_accum__write_binary(mb_accum_t *accum, FILE *fp);


#endif