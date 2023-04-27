/* binary IO routines for at_mb_t */
#ifndef AT__MB_IO_BINARY__SRC_H__
#define AT__MB_IO_BINARY__SRC_H__

#include "mb_io_binary.h"

/* implementation headers */
#include "../mb_basic.h" /* at_mb__clear() */
#include "../sm/mb_sm.h"
#include "../accum/mb_accum.h"

#include "../../zcom/endn/endn.h"

static int at_mb__read_binary_low_level(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double *beta,
    FILE *fp, int ver, int endn)
{
  int itmp;

  if (mb == NULL) {
    fprintf(stderr, "passing null pointer to at_mb__read_binary_low_level\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* clear data before reading */
  at_mb__clear(mb);
  at_langevin__clear(langevin);

  /* n: number of temperature bins */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->n) {
    fprintf(stderr, "mb->n mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->n, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* m: maximal number of bins in a window */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->accum->m) {
    fprintf(stderr, "mb->m mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->accum->m, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  {
    int mb_order = 1;

    /* order: order, should be 1 */
    if (zcom_endn__fread(&mb_order, sizeof(mb_order), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading mb_order\n");
      goto ERR;
    }
    if ( !(mb_order == 1) ) {
      fprintf(stderr, "mb->order: failed validation: mb_order == 1\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  /* flags: combination of flags */
  if (zcom_endn__fread(&mb->flags, sizeof(mb->flags), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->flags\n");
    goto ERR;
  }
  /* use_winaccum */
  if (zcom_endn__fread(&mb->accum->use_winaccum, sizeof(mb->accum->use_winaccum), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->use_winaccum\n");
    goto ERR;
  }
  /* cnt_int: number of additional integer variables to be written to binary file */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->cnt_int) {
    fprintf(stderr, "mb->cnt_int mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->cnt_int, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* cnt_dbl: number of additional double variables to be written to binary file */
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->cnt_dbl) {
    fprintf(stderr, "mb->cnt_dbl mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->cnt_dbl, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  {
    /* beta: current value of beta */
    if (zcom_endn__fread(beta, sizeof(*beta), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading beta\n");
      goto ERR;
    }

    if ( !(*beta >= mb->bmin && *beta <= mb->bmax) ) {
      fprintf(stderr, "beta: failed validation: beta >= mb->bmin && beta <= mb->bmax\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }

    if (*beta > mb->bmax - 1e-5) {
      *beta = mb->bmax - 1e-5;
    } else if (*beta < mb->bmin + 1e-5) {
      *beta = mb->bmin + 1e-5;
    }

  }


  /* total_visits: total number of visits, number of tempering */
  if (zcom_endn__fread(&mb->total_visits, sizeof(mb->total_visits), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->total_visits\n");
    goto ERR;
  }
  if ( !(mb->total_visits > 0) ) {
    fprintf(stderr, "mb->total_visits: failed validation: mb->total_visits > 0\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_base: current generic shrink amplitude */
  if (zcom_endn__fread(&mb->shk->base, sizeof(mb->shk->base), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->shk_base\n");
    goto ERR;
  }

  if (at_langevin__read_binary(langevin, fp, endn) != 0) {
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->n > 0)
    && (zcom_endn__fread(mb->barr, sizeof(*(mb->barr)), mb->n, fp, endn) != (size_t) (mb->n))) {
    fprintf(stderr, "error in reading mb->barr, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  /* et: bin-averaged internal energy */
  /*
  if ((mb->n > 0)
    && (zcom_endn__fread(mb->iie->et, sizeof(*(mb->iie->et)), mb->n, fp, endn) != (size_t) (mb->n))) {
    fprintf(stderr, "error in reading mb->et, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  */

  if (at_mb_accum__read_binary(mb->accum, fp, endn) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  at_mb__clear(mb);
  at_langevin__clear(langevin);
  return -1;
}



int at_mb__read_binary(
    at_mb_t *mb,
    at_langevin_t *langevin, 
    double *beta,
    const char *fname,
    int *pver)
{
  FILE *fp;
  int ver;
  int itmp;
  int i;
  int endn;

  if ((fp = fopen(fname, "rb")) == NULL) {
    fprintf(stderr, "cannot read binary file [%s].\n", fname);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* determine file endian */
  if ((endn = zcom_endn__rmatchi(&itmp, sizeof(int), fp)) < 0) {
    fprintf(stderr, "itmp 0x%X cannot match sizeof(int) 0x%X\n",
        (unsigned) itmp, (unsigned) sizeof(int));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (zcom_endn__fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != (int) sizeof(double)) {
    fprintf(stderr, "(int) sizeof(double) mismatch, expect: %d, read: %d, pos: %#lx\n",
        (int) sizeof(double), itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (zcom_endn__fread(&ver, sizeof(ver), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading ver\n");
    goto ERR;
  }
  if (pver != NULL) *pver = ver;

  /* call low level read function for members */
  i = at_mb__read_binary_low_level(mb, langevin, beta, fp, ver, endn);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}



static int at_mb__write_binary_low_level(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double beta,
    FILE *fp, int ver)
{
  if (mb == NULL) {
    fprintf(stderr, "passing null pointer to at_mb__write_binary_low_level\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  /* n: number of temperature bins */
  if (zcom_endn__fwrite(&mb->n, sizeof(mb->n), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->n\n");
    goto ERR;
  }
  /* m: maximal number of bins in a window */
  if (zcom_endn__fwrite(&mb->accum->m, sizeof(mb->accum->m), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->m\n");
    goto ERR;
  }

  {
    int mb_order = 1;

    if (zcom_endn__fwrite(&mb_order, sizeof(mb_order), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing mb_order\n");
      goto ERR;
    }
  }

  /* flags: combination of flags */
  if (zcom_endn__fwrite(&mb->flags, sizeof(mb->flags), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->flags\n");
    goto ERR;
  }
  /* use_winaccum */
  if (zcom_endn__fwrite(&mb->accum->use_winaccum, sizeof(mb->accum->use_winaccum), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->use_winaccum\n");
    goto ERR;
  }
  /* cnt_int: number of additional integer variables to be written to binary file */
  if (zcom_endn__fwrite(&mb->cnt_int, sizeof(mb->cnt_int), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->cnt_int\n");
    goto ERR;
  }
  /* cnt_dbl: number of additional double variables to be written to binary file */
  if (zcom_endn__fwrite(&mb->cnt_dbl, sizeof(mb->cnt_dbl), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->cnt_dbl\n");
    goto ERR;
  }

  {
    /* beta: current value of beta */
    if (zcom_endn__fwrite(&beta, sizeof(beta), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing mb->beta\n");
      goto ERR;
    }
  }

  /* total_visits: total number of visits, number of tempering */
  if (zcom_endn__fwrite(&mb->total_visits, sizeof(mb->total_visits), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->total_visits\n");
    goto ERR;
  }
  /* shk_base: current generic shrink amplitude */
  if (zcom_endn__fwrite(&mb->shk->base, sizeof(mb->shk->base), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->shk->base\n");
    goto ERR;
  }

  if (at_langevin__write_binary(langevin, fp) != 0) {
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->n > 0)
    && (zcom_endn__fwrite(mb->barr, sizeof(*(mb->barr)), mb->n, fp, 1) != (size_t) (mb->n))) {
    fprintf(stderr, "error in writing mb->barr, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  /* et: bin-averaged internal energy */
  /*
  if ((mb->n > 0)
    && (zcom_endn__fwrite(mb->iie->et, sizeof(*(mb->iie->et)), mb->n, fp, 1) != (size_t) (mb->n))) {
    fprintf(stderr, "error in writing mb->et, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  */

  if (at_mb_accum__write_binary(mb->accum, fp) != 0) {
    goto ERR;
  }

  return 0;
ERR:
  return -1;
}



int at_mb__write_binary(
    at_mb_t *mb,
    at_langevin_t *langevin,
    double beta,
    const char *fname,
    int ver)
{
  FILE *fp;
  int i;
  int size;

  if ((fp = fopen(fname, "wb")) == NULL) {
    fprintf(stderr, "cannot write binary file [%s].\n", fname);
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  size = (int) sizeof(int);
  if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing size\n");
    goto ERR;
  }
  size = (int) sizeof(double);
  if (zcom_endn__fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing size\n");
    goto ERR;
  }
  if (zcom_endn__fwrite(&ver, sizeof(ver), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing ver\n");
    goto ERR;
  }

  /* call low level write function for members */
  i = at_mb__write_binary_low_level(mb, langevin, beta, fp, ver);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}


#endif
