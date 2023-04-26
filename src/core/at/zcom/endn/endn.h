#ifndef ZCOM__ENDN_H__
#define ZCOM__ENDN_H__



#include <stdio.h>
#include <string.h>

#include "../def/def.h"

ZCOM_STRCLS int zcom_endn__system(void);
ZCOM_STRCLS size_t zcom_endn__fwrite(void *ptr, size_t size, size_t n, FILE *fp, int endn);
ZCOM_STRCLS size_t zcom_endn__fread(void *ptr, size_t size, size_t n, FILE *fp, int flip);
ZCOM_STRCLS int zcom_endn__rmatch(void *src, const void *ref, size_t size, FILE *fp);
ZCOM_STRCLS int zcom_endn__rmatchi(int *src, int iref, FILE *fp);

#endif