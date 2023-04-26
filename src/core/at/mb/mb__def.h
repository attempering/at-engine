/* multiple-bin estimators of thermodynamical quantities */
#ifndef AT__MB_DEF_H__
#define AT__MB_DEF_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "sm/mb_sm__def.h"
#include "betadist/mb_betadist__def.h"
#include "shk/mb_shk__def.h"
#include "win/mb_win__def.h"
#include "accum/mb_accum__def.h"
#include "zerofiller/mb_zerofiller__def.h"
#include "integrator/mb_integrator__def.h"
#include "iie/mb_iie__def.h"

typedef struct zcom_ssm_t_ zcom_ssm_t;

/* multiple-bin estimator parameters */
typedef struct at_mb_t_ {
  double    boltz;    /* Boltzmann constant */

  double    bmin;     /* minimal beta (highest temperature) */
  double    bmax;     /* maximal beta (lowest temperature) */
  double    bdel;     /* bin size of beta */
  int       n;        /* number of temperature bins */
  double    *barr;    /* temperature array */
  double    beta;     /* current value of beta */
  unsigned  flags;    /* combination of flags */

  int       nstrefresh;   /* interval of recalculating et for all temperature */
  int       nst_save_av;  /* interval of writing mbav and ze files */
  int       av_binary;    /* use binary format in mbav file */
  char      *av_file;     /* name of mbav file */
  char      *ze_file;     /* name of ze file */
  int       wze_reps;     /* number of iterations before writing ze file */
  double    *visits;      /* number of visits */
  double    total_visits; /* total number of visits, number of tempering */

  /* overall beta distribution */
  at_mb_betadist_t betadist[1];

  /* adaptive averaging related parameters */
  at_mb_shk_t  shk[1];

  /* window settings */
  at_mb_win_t win[1];

  /* accumulators */
  at_mb_accum_t accum[1];

  /* zero filler */
  at_mb_zerofiller_t zerofiller[1];

  /* integrator */
  at_mb_integrator_t integrator[1];

  at_mb_iie_t  iie[1];

  zcom_ssm_t     *ssm;     /* string allocator */

  int       cnt_int;  /* number of additional integer variables to be written to binary file */
  int       cnt_dbl;  /* number of additional double variables to be written to binary file */

} at_mb_t;

#define   MB_USE_WIN_ACCUM       0x00000001    /* use adaptive averaging */
#define   MB_CV         0x00000002    /* compute heat capacity */
#define   MB_SYMWIN     0x00000004    /* use symmetrical windows */
#define   MB_SINGLE_BIN 0x00000020    /* use the single-bin estimator */
#define   MB_VERBOSE    0x00001000    /* be verbose */



#endif


