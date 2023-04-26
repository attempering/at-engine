#ifndef AT__LANGEVIN_IO_H__
#define AT__LANGEVIN_IO_H__

#include "langevin__def.h"
#include <stdio.h>

void at_langevin__manifest(const at_langevin_t *langevin, FILE *fp, int arrcnt);

int at_langevin__read_binary(at_langevin_t *langevin, FILE *fp, int endn);

int at_langevin__write_binary(at_langevin_t *langevin, FILE *fp);

#endif
