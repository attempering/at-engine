// gcc -g _test.c -lm && valgrind ./a.out
#include "../../zcom/util/util__src.h"
#include "../../zcom/ssm/ssm__src.h"
#include "../../zcom/opt/zcom_opt__src.h"
#include "../../zcom/cfg/zcom_cfg__src.h"
#include "mb_win__src.h"



static void mock_mb__init(at_mb_t *mb, double bmin, double bmax, double bdel)
{
  int i, n;

  n = (int) ((bmax - bmin)/bdel + 0.5);
  mb->n = n;
  mb->bmin = bmin;
  mb->bmax = bmax;
  mb->bdel = bdel;
  mb->barr = calloc(n+1, sizeof(double));

  for (i = 0; i <= n; i++) {
    mb->barr[i] = bmin + bdel * i;
  }

  mb->flags = MB_SYMWIN;
}


static void mock_mb__finish(at_mb_t *mb)
{
  free(mb->barr);
}



int main(void)
{
  at_mb_t mb[1];
  at_mb_win_t win[1];

  mock_mb__init(mb, 0.2, 0.4, 0.001);
  at_mb_win__cfg_init(win, NULL, mb);
  at_mb_win__finish(win);
  mock_mb__finish(mb);

  return 0;
}