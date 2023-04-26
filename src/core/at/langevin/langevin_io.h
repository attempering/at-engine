#ifndef AT__LANGEVIN_IO_H__
#define AT__LANGEVIN_IO_H__

#include "langevin__def.h"
#include <stdio.h>

void langevin_manifest(const langevin_t *langevin, FILE *fp, int arrcnt);

int langevin_read_binary(langevin_t *langevin, FILE *fp, int endn);

int langevin_write_binary(langevin_t *langevin, FILE *fp);

#endif
