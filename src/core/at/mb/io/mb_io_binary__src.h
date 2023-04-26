/* binary IO routines for mb_t */
#ifndef AT__MB_IO_BINARY__SRC_H__
#define AT__MB_IO_BINARY__SRC_H__

#include "mb_io_binary.h"

/* implementation headers */
#include "../mb_basic.h" /* mb__clear() */
#include "../../sm/sm.h"
#include "../accum/mb_accum.h"

#include "../../zcom/endn/endn.h"

static int mb_read_binary_low_level(mb_t *mb, langevin_t *langevin, FILE *fp, int ver, int endn)
{
  int itmp;

  if (mb == NULL) {
    fprintf(stderr, "passing null pointer to mb_read_binary_low_level\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }

  /* clear data before reading */
  mb__clear(mb);
  langevin_clear(langevin);

  /* n: number of temperature bins */
  if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
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
  if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
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
    if (endn_fread(&mb_order, sizeof(mb_order), 1, fp, endn) != 1) {
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
  if (endn_fread(&mb->flags, sizeof(mb->flags), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->flags\n");
    goto ERR;
  }
  /* use_winaccum */
  if (endn_fread(&mb->accum->use_winaccum, sizeof(mb->accum->use_winaccum), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->use_winaccum\n");
    goto ERR;
  }
  /* cnt_int: number of additional integer variables to be written to binary file */
  if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
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
  if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != mb->cnt_dbl) {
    fprintf(stderr, "mb->cnt_dbl mismatch, expect: %d, read: %d, pos: %#lx\n",
        mb->cnt_dbl, itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  /* beta: current value of beta */
  if (endn_fread(&mb->beta, sizeof(mb->beta), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->beta\n");
    goto ERR;
  }
  if ( !(mb->beta >= mb->bmin && mb->beta <= mb->bmax) ) {
    fprintf(stderr, "mb->beta: failed validation: mb->beta >= mb->bmin && mb->beta <= mb->bmax\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  mb->beta = (mb->beta >= mb->bmax - 1e-5) ? (mb->bmax - 1e-5) : mb->beta;
  mb->beta = (mb->beta <= mb->bmin + 1e-5) ? (mb->bmin + 1e-5) : mb->beta;
  /* total_visits: total number of visits, number of tempering */
  if (endn_fread(&mb->total_visits, sizeof(mb->total_visits), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->total_visits\n");
    goto ERR;
  }
  if ( !(mb->total_visits > 0) ) {
    fprintf(stderr, "mb->total_visits: failed validation: mb->total_visits > 0\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }

  /* shk_base: current generic shrink amplitude */
  if (endn_fread(&mb->shk->base, sizeof(mb->shk->base), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading mb->shk_base\n");
    goto ERR;
  }

  if (langevin_read_binary(langevin, fp, endn) != 0) {
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->n > 0)
    && (endn_fread(mb->barr, sizeof(*(mb->barr)), mb->n, fp, endn) != (size_t) (mb->n))) {
    fprintf(stderr, "error in reading mb->barr, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  /* et: bin-averaged internal energy */
  /*
  if ((mb->n > 0)
    && (endn_fread(mb->iie->et, sizeof(*(mb->iie->et)), mb->n, fp, endn) != (size_t) (mb->n))) {
    fprintf(stderr, "error in reading mb->et, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  */

  if (mb_accum__read_binary(mb->accum, fp, endn) != 0) {
    goto ERR;
  }

  return 0;

ERR:
  mb__clear(mb);
  langevin_clear(langevin);
  return -1;
}



int mb__read_binary(mb_t *mb, langevin_t *langevin, const char *fname, int *pver)
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
  if ((endn = endn_rmatchi(&itmp, sizeof(int), fp)) < 0) {
    fprintf(stderr, "itmp 0x%X cannot match sizeof(int) 0x%X\n",
        (unsigned) itmp, (unsigned) sizeof(int));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading itmp\n");
    goto ERR;
  }
  if (itmp != (int) sizeof(double)) {
    fprintf(stderr, "(int) sizeof(double) mismatch, expect: %d, read: %d, pos: %#lx\n",
        (int) sizeof(double), itmp, (unsigned long) ftell(fp));
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    goto ERR;
  }
  if (endn_fread(&ver, sizeof(ver), 1, fp, endn) != 1) {
    fprintf(stderr, "error in reading ver\n");
    goto ERR;
  }
  if (pver != NULL) *pver = ver;

  /* call low level read function for members */
  i = mb_read_binary_low_level(mb, langevin, fp, ver, endn);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}



static int mb_write_binary_low_level(mb_t *mb, langevin_t *langevin, FILE *fp, int ver)
{
  if (mb == NULL) {
    fprintf(stderr, "passing null pointer to mb_write_binary_low_level\n");
    fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
    return -1;
  }
  /* n: number of temperature bins */
  if (endn_fwrite(&mb->n, sizeof(mb->n), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->n\n");
    goto ERR;
  }
  /* m: maximal number of bins in a window */
  if (endn_fwrite(&mb->accum->m, sizeof(mb->accum->m), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->m\n");
    goto ERR;
  }

  {
    int mb_order = 1;

    if (endn_fwrite(&mb_order, sizeof(mb_order), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing mb_order\n");
      goto ERR;
    }
  }

  /* flags: combination of flags */
  if (endn_fwrite(&mb->flags, sizeof(mb->flags), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->flags\n");
    goto ERR;
  }
  /* use_winaccum */
  if (endn_fwrite(&mb->accum->use_winaccum, sizeof(mb->accum->use_winaccum), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->use_winaccum\n");
    goto ERR;
  }
  /* cnt_int: number of additional integer variables to be written to binary file */
  if (endn_fwrite(&mb->cnt_int, sizeof(mb->cnt_int), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->cnt_int\n");
    goto ERR;
  }
  /* cnt_dbl: number of additional double variables to be written to binary file */
  if (endn_fwrite(&mb->cnt_dbl, sizeof(mb->cnt_dbl), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->cnt_dbl\n");
    goto ERR;
  }
  /* beta: current value of beta */
  if (endn_fwrite(&mb->beta, sizeof(mb->beta), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->beta\n");
    goto ERR;
  }
  /* total_visits: total number of visits, number of tempering */
  if (endn_fwrite(&mb->total_visits, sizeof(mb->total_visits), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->total_visits\n");
    goto ERR;
  }
  /* shk_base: current generic shrink amplitude */
  if (endn_fwrite(&mb->shk->base, sizeof(mb->shk->base), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing mb->shk->base\n");
    goto ERR;
  }

  if (langevin_write_binary(langevin, fp) != 0) {
    goto ERR;
  }

  /* barr: temperature array */
  if ((mb->n > 0)
    && (endn_fwrite(mb->barr, sizeof(*(mb->barr)), mb->n, fp, 1) != (size_t) (mb->n))) {
    fprintf(stderr, "error in writing mb->barr, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  /* et: bin-averaged internal energy */
  /*
  if ((mb->n > 0)
    && (endn_fwrite(mb->iie->et, sizeof(*(mb->iie->et)), mb->n, fp, 1) != (size_t) (mb->n))) {
    fprintf(stderr, "error in writing mb->et, n = mb->n(%d)\n", mb->n);
    goto ERR;
  }
  */

  if (mb_accum__write_binary(mb->accum, fp) != 0) {
    goto ERR;
  }

  return 0;
ERR:
  return -1;
}



int mb__write_binary(mb_t *mb, langevin_t *langevin, const char *fname, int ver)
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
  if (endn_fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing size\n");
    goto ERR;
  }
  size = (int) sizeof(double);
  if (endn_fwrite(&size, sizeof(size), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing size\n");
    goto ERR;
  }
  if (endn_fwrite(&ver, sizeof(ver), 1, fp, 1) != 1) {
    fprintf(stderr, "error in writing ver\n");
    goto ERR;
  }

  /* call low level write function for members */
  i = mb_write_binary_low_level(mb, langevin, fp, ver);

  fclose(fp);
  return i;
ERR:
  fclose(fp);
  return -1;
}


#endif
