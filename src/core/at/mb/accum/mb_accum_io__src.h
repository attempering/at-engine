#ifndef AT__MB_ACCUM_IO__SRC_H__
#define AT__MB_ACCUM_IO__SRC_H__


#include "mb_accum.h"

#include "../../sm/sm.h"

#include "../../zcom/util/util.h"
#include "../../zcom/cfg/cfg.h"
#include "../../zcom/endn/endn.h"


void mb_accum__manifest(const mb_accum_t *accum, FILE *fp, int arrmax)
{
  int i, pacnt;

  /* sums: normal data */
  if (accum->sums != NULL) {
    fprintf(fp, "accum->sums: sm_t array of accum->n:");

    for (i = accum->n-1; i >= 0; i--) {
      if (accum->sums[i].s != 0) {
        break;
      }
    }

    if (i >= 0) {
      fprintf(fp, "\n");
      for (i = 0; i < accum->n; i++) {
        if (i == arrmax && i < accum->n-arrmax) {
          fprintf(fp, "\n...\n");
        }
        if (arrmax >= 0 && i >= arrmax && i < (accum->n-arrmax)) continue;
        fprintf(fp, "accum->sums[%d]:\n", i);
        sm__manifest(accum->sums+i, fp, arrmax);
      }
    } else {
      fprintf(fp, " {0}\n");
    }
  }

  /* use_winaccum */
  fprintf(fp, "accum->use_winaccum: int, %4d\n", accum->use_winaccum);

  /* win_total: total of sum.s over a multiple-bine temperature window */
  fprintf(fp, "accum->win_total: dynamic array of accum->n: ");
  for (i = accum->n-1; i >= 0; i--) if (fabs(accum->win_total[i]) > 1e-30) break;
  if (i >= 0) {
    if ((arrmax < 0 || arrmax > 3) && accum->n > 6)
      fprintf(fp, "\n");
    for (pacnt = 0, i = 0; i < accum->n; i++) {
      if (i == arrmax && i < accum->n-arrmax) {
        if (arrmax > 3 && pacnt % 10 != 0) fprintf(fp, "\n");
        fprintf(fp, "..., ");
        if (arrmax > 3) fprintf(fp, "\n");
      }
      if (arrmax >= 0 && i >= arrmax && i < (accum->n-arrmax)) continue;
      fprintf(fp, "%g, ", accum->win_total[i]);
      if (++pacnt % 10 == 0) fprintf(fp, "\n");
    }
    if (pacnt % 10 != 0) fprintf(fp, "\n");
  } else {
    fprintf(fp, " {0}\n");
  }

}



int mb_accum__read_binary(mb_accum_t *accum, FILE *fp, int endn)
{
  int i, itmp;

  /* sums: normal data */
  exit_if (accum->sums == NULL,
      "accum->sums is null\n");

  for (i = 0; i < accum->n; i++) {
    if (endn_fread(&itmp, sizeof(itmp), 1, fp, endn) != 1) {
      fprintf(stderr, "error in reading itmp\n");
      goto ERR;
    }
    if (itmp != i) {
      fprintf(stderr, "i mismatch, expect: %d, read: %d, pos: %#lx\n",
          i, itmp, (unsigned long) ftell(fp));
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
    if (0 != sm__read_binary(accum->sums+i, fp, endn)) {
      fprintf(stderr, "error reading object accum->sums+i\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  mb_accum_winaccum__read_binary(accum->winaccum, fp, endn);

  return 0;
ERR:
  return -1;
}



int mb_accum__write_binary(mb_accum_t *accum, FILE *fp)
{
  int i;
  sm_t *sm;

  /* sums: normal data */
  exit_if (accum->sums == NULL,
      "accum->sums is null\n");

  for (i = 0; i < accum->n; i++) {
    if (endn_fwrite(&i, sizeof(i), 1, fp, 1) != 1) {
      fprintf(stderr, "error in writing i\n");
      goto ERR;
    }

    sm = mb_accum__get_sums(accum, i);

    if (0 != sm__write_binary(sm, fp)) {
      fprintf(stderr, "error writing object accum->sums+i\n");
      fprintf(stderr, "Location: %s:%d\n", __FILE__, __LINE__);
      goto ERR;
    }
  }

  mb_accum_winaccum__write_binary(accum->winaccum, fp);

  return 0;
ERR:

  return -1;
}



#endif
