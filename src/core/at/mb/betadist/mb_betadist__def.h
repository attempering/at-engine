#ifndef AT__MB_BETADIST__DEF_H__
#define AT__MB_BETADIST__DEF_H__

/* parameters for the overall beta disribution */
typedef struct mb_betadist_t_ {
  int       mode;         /* mode 0: disable 1: Gaussian 2:exponential */
  double    ens_exp;      /* ensemble exponent of beta */
  double    beta0;        /* beta0 in Gaussian distribution */
  double    invsigma2;    /* 1/sigma^2 in Gaussian distribution */
  double    c;            /* c in exponential distribution */
  double   *ens_w;        /* array of ensemble weights at bin boundaries */
  int       n;            /* number of beta bins == mb->n */
  double    beta_min;     /* minimal beta == mb->bmin */
  double    beta_max;     /* maximal beta == mb->bmax */
} mb_betadist_t;

#endif
