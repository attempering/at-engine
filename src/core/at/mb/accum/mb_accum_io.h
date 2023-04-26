#ifndef AT__MB_ACCUM_IO_H__
#define AT__MB_ACCUM_IO_H__


#include "mb_accum__def.h"
#include "../mb__def.h"
#include "../win/mb_win.h"
#include "../shk/mb_shk.h"
#include <stdio.h>

void at_mb_accum__manifest(const at_mb_accum_t *accum, FILE *fp, int arrmax);

int at_mb_accum__read_binary(at_mb_accum_t *accum, FILE *fp, int endn);

int at_mb_accum__write_binary(at_mb_accum_t *accum, FILE *fp);


#endif