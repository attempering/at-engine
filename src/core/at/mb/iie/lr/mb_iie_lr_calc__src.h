#ifndef AT__MB_IIE_LR_CALC__SRC_H__
#define AT__MB_IIE_LR_CALC__SRC_H__


#include "mb_iie_lr_calc.h"

/* implementation dependent headers */
#include "../../mb_basic.h"
#include "../../../../zcom/zcom.h"
#include "../../accum/mb_accum.h"
#include "../zerofiller/mb_iie_zerofiller.h"



/* initialization for a special case */
void at_mb_iie_lr__init_instance(at_mb_iie_lr_t *lr,
    int type, int win_div,
    int ib, int js, int jt)
{
  lr->type = type;
  lr->win_div = win_div;

  zcom_util__exit_if (0 > js || js >= jt || jt > lr->mb->n,
    "bad window [%d, %d)", js, jt);

  lr->ib = ib;
  lr->js = js;
  lr->jt = jt;

  if (lr->type == MB_IIE_LR__TYPE_BIN) {

    if (lr->win_div == MB_IIE_LR__WIN_DIV_PAPER) {
      lr->jb = ib + 1;
    } else {
      lr->jb = ib;
    }

  } else {

    lr->jb = ib;

  }

  // The origin of the accumulators is either
  //   mb->accum->sums
  // for plain averages, or
  //   mb->accum->winaccum->items[ib].sums - .js
  // for windowed averages
  //
  lr->sm0 = at_mb_accum__get_proper_sums0_and_winaccum_item(
      lr->mb->accum, ib, &lr->winaccum_item);

  lr->s0[0] = lr->s0[1] = 0.0;
  lr->s1[0] = lr->s1[1] = 0.0;
  lr->t1[0] = lr->t1[1] = 0.0;
  lr->tb = 0.0;

  lr->imbalance = 0.0;
  lr->success = 0;
}



// filling zf->vals[] and zf->has_vals[]
// from lr->sm0[lr->js .. lr->jt]
static void at_mb_iie_lr__collect_raw_array(at_mb_iie_lr_t *lr, at_mb_iie_zerofiller_t *zf)
{
  double el, var;
  int j;

  for (j = lr->js; j < lr->jt; j++) { /* loop over bins */
    at_mb_sm_t *sm = lr->sm0 + j;

    /* skip an empty bin */
    if (sm->s < MB_ACCUM_MIN_SIZE) {

      zf->has_vals[j] = 0;

    } else {

      el = sm->se / sm->s;
      var = sm->se2 / sm->s;

      zf->has_vals[j] = 1;
      zf->vals[j].w = sm->s;
      zf->vals[j].mean = el;
      zf->vals[j].var = var;

    }

  }

  //getchar();

}



/**
 * collect moments from the left and right windows
 *
 * The left window is defined as [js, jb).
 * The right window is defined as [jb, jt).
 *
 * jb = ib in the legacy-code convention,
 * jb = ib + 1 in the paper convention.
 * 
 **/
void at_mb_iie_lr__collect_moments(at_mb_iie_lr_t *lr)
{
  double el, var;
  int j, jx, lr_id;
  at_mb_iie_zerofiller_t *zf = lr->zerofiller;

  at_mb_iie_lr__collect_raw_array(lr, zf);

  at_mb_iie_zerofiller__fill_missing(zf, lr->js, lr->jt - 1);

  for (j = lr->js; j < lr->jt; j++) { /* loop over bins */

    if (j < lr->jb) {
      jx = lr->js;
      lr_id = 0;
    } else {
      jx = lr->jt;
      lr_id = 1;
    }

    el = zf->vals[j].mean;
    var = zf->vals[j].var;


    /* in the regularized case, s0 is simply the number of bins
      * in a window:
      *
      *  s0[0] = i - js
      *  s0[1] = jt - i
      *
      * */
    lr->s0[lr_id] += 1;
    lr->s1[lr_id] += el;

    lr->t1[lr_id] += var * (j + 0.5 - jx);

    /*
       *tb is defined as phi_t(beta) <dE^2>,
       evaluated at the midpoint of the bin.

       In the paper convention (jb == ib + 1),

                               beta - beta[ib]
        phi_t(beta) = - -----------------------
                         beta[ib+1] - beta[ib]

       which is reduced to -0.5.

       In the legacy-code convention (jb == ib),

                         beta[ib+1] - beta
        phi_t(beta) = + -----------------------
                         beta[ib+1] - beta[ib]

       which becomes +0.5.

     */
    if (j == lr->ib && lr->type == MB_IIE_LR__TYPE_BIN) {
      if (lr->win_div == MB_IIE_LR__WIN_DIV_LEGACY) {
        lr->tb = 0.5 * var;
      } else if (lr->win_div == MB_IIE_LR__WIN_DIV_PAPER) {
        lr->tb = -0.5 * var;
      } else {
        fprintf(stderr, "Fatal: unknown window convention ib %d, window boundary %d\n",
            lr->ib, lr->jb);
        exit(1);
      }
    }

  }

  /*
  if (lr->type == MB_IIE_LR__TYPE_GRID) {
    printf("tb %g, ib %d, %d, s0 %g, %g, s1 %g, %g, t1 %g, %g\n",
        lr->tb, lr->ib, lr->jb,
        lr->s0[0], lr->s0[1],
        lr->s1[0], lr->s1[1],
        lr->t1[0], lr->t1[1]);
  }
  */

}



/**
 * compute an estimate of the thermodynamic quantity
 * by linearly combining averages from the left and right windows
 *
 * The thermodynamic quantity can be
 *  * bin-average potential energy,
 *  * bin-boundary potential energy,
 *  * the bin-boundary energy fluctuation <dE^2>
 *
 */
double at_mb_iie_lr__balance_moments(at_mb_iie_lr_t *lr)
{
  double cm, cp; /* combination coefficients, cm for the left window, cp for the right */
  double num, den, del;
  double est_value;
  int ip, im;
  int fallback_activated = 0;

  const double min_e2sum = 1e-3;
  const double min_csum = 1e-8;

  double min_size = lr->min_size;

  cm = cp = 1.0;

  lr->success = 0;
  lr->quality = 0;

  /* if there isn't enough data points in the entire window
   * return 0.0 */
  if (fabs(lr->s0[0] + lr->s0[1]) < min_size) {
    return 0.0;
  }

  im = lr->jb - lr->js;
  ip = lr->jt - lr->jb;

#ifdef MB_DBG__
  printf("im %d, ip %d\n", im, ip);
#endif

  /*
    The equation we wish to solve is

      Int_{beta'} phi(beta') <dE^2>(beta') d beta' = 0

    In discrete variables, it reads (after dividing
    the left-hand side by the bin width)

      sum_{ j = [js, jt) } phi_s[j] <dE^2>[j]
                         + phi_t[ib] <dE^2>[ib] = 0

    or

      sum_{ j = [js, jb) } phi_s[j] <dE^2>[j]
    + sum_{ j = [jb, jt) } phi_s[j] <dE^2>[j]
                         + phi_t[ib] <dE^2>[ib] = 0

    where, for j in [js, jb),

                     beta(j) - beta[js]         j + 0.5 - js      am
      phi_s[j] = am --------------------- = am --------------- = ---- (j + 0.5 - js),
                     beta[jb] - beta[js]          jb - js         im

    for j in [jb, jt),

                     beta(j) - beta[jt]         j + 0.5 - jt      ap
      phi_s[j] = ap --------------------- = ap --------------- = ---- (j + 0.5 - jt),
                     beta[jt] - beta[jb]          jt - jb         ip

    where, am and ap are, respectively, the a- and a+ in the paper,
    and they satisfy the condition:

      am + ap = 1,

    beta(j) indicates the midpoint value of bin j:

      beta(j) = beta[j] + 0.5 * beta_bin_width.

    The last two factors (j + 0.5 - js) and (j + 0.5 - jt) of phi_s[j]
    have been absorbed in the definitions of t1[0] and t1[1] respectively.

    For j in [js, jb),

        sum_{ j = [js, jb) } phi_s[j] <dE^2>[j]
      = sum_{ j = [js, jb) } (am/im) (j + 0.5 - js) <dE^2>[j]
      = (am/im) t1[0]

    For j in [jb, jt),

        sum_{ j = [jb, jt) } phi_s[j] <dE^2>[j]
      = sum_{ j = [jb, jt) } (ap/ip) (j + 0.5 - jt) <dE^2>[j]
      = (ap/ip) t1[1]

    The phi_t[j] term vanishes unless j = ib, in which case

                     beta(j) - beta[ib]      j == ib
      phi_t[j] = - ----------------------- =========== -0.5 (paper convention)
                    beta[ib+1] - beta[ib]

    In the legacy code convention,

                   beta[ib+1] - beta(j)    j == ib
      phi_t[j] = ----------------------- =========== 0.5 (legacy-code convention)
                   beta[ib+1] - beta[ib]


    And tb is defined as

      tb = phi_t[ib] <dE^2>[ib].

    So the equation we have is

       am           ap
      ---- t1[0] + ---- t1[1] + tb = 0.
       im           ip

    With a uniform window, we expect am and ap to be roughly
    proportional to im and ip, respectively.

    If the quantity we wish average varies smoothly over the window,
    am and ap should be proportional to im and ip respectively.

    To better control the errors, we will introduce a pair of
    normalized variables as

               im + ip
      cm = am ---------
                  im

               im + ip
      cm = ap ---------
                  ip

    Then, in terms of bm and bp, we have

      cm * t1[0] + cp * t1[1] = -tb * (im + ip) = -tb * (jt - js)
      cm * im    + cp * ip    = im + ip = jt - js

    The solution to the above linear equations is

                tb * ip + t1[1]
      cm = - ------------------------- (im + ip)
              ip * t1[0] - im * t1[1]

              t1[0] + tb * im
      cp = ------------------------- (im + ip)
            ip * t1[0] - im * t1[1]

    And the difference of the two is

                  t1[0] + t1[1] + tb * (im + ip)
      cp - cm  = -------------------------------- (im + ip)
                     ip * t1[0] - im * t1[1]

               = del * (im + ip)

    where we have defined the fraction as del,
    which is also the variable name in the code.
    The numerator and denominator are respectively `num`, `den`
    in the code.

    With the definition of del, we can rewrite cp and cm
    in terms of del. For example, for cp, we have

      cm * im + cp * ip = im + ip
      cp * ip - cm * ip = (im + ip) (del * ip)

    Adding up the two equations, we have

      cm (im + ip) = (im + ip) (1 - del * ip)

    So

      cm = 1 - del * ip

    Similarly,

      cp = 1 + del * im

    In the legacy code, cm and cp are variables a and b respectively.

    The advantage of using cm and cp instead of am and ap
    is that we expect cm and cp to be roughly the same,
    and they should be close to 1.0, as the average
    quantity usually varies smoothly over the window
    If our data show a large discrepancy between the two,
    or even a negative value of either cm or cp,
    it is likely due to insufficient data.

    From cm and cp, we can compute am and ap as

             cm im
      am = ---------,
            im + ip

             cp ip
      ap = ---------.
            im + ip

    In other words, am and ap are normalized versions of `cm*im` and `cp*ip`.
    In the code, however, we will not compute am and ap directly,
    but use the un-normalized `cm*im` and `cp*ip`.


    Ultimately, we want to evaluate the integral

      Et(beta) = Int_{beta'} <E>(beta') phi_s'(beta') d beta'

               = sum_{ j in [js, jt) } E[j] phi_s'[j] delta-beta

                                                am
               = sum_{ j in [js, jb) } E[j] --------------------- delta-beta
                                             beta[jb] - beta[js]

                                                ap
               + sum_{ j in [ib, jw) } E[j] --------------------- delta-beta
                                             beta[jt] - beta[jb]

               = [ sum_{ j in [js, ib) } E[j] ] am / im
               + [ sum_{ j in [ib, jt) } E[j] ] ap / ip

               = E_sum[0] bm + E_sum[1] bp

    In the regularized case,
    E_sum[0] is interpreted as s1[0], with s0[0] being im;
    and E_sum[1] = s1[1], with s0[1] = ip.

                       s1[0]               s1[1]
      Et(beta) = ( im ------- ) bm  + (ip ------- ) bp
                       s0[0]               s0[1]

               = E_av[0] am + E_av[1] ap.

    E_av[0] and E_av[1] are the averages from the left and
    right half-windows, respectively.
    From this equation, it is readily seen that
    am and ap serve as the coefficients of linear combinations
    for E_av[0] and E_av[1] respectively.
    Naturally, the condition am + ap = 1 has to hold.

    In terms of the un-normalized weights

                  (cm im) E_av[0] + (cp ip) E_av[1]
      Et(beta) = -----------------------------------
                      cm im + cp ip

                  cm s1[0] + cp s1[1]
               = ---------------------
                  cm s0[0] + cp s0[1]

                  a * s1[0] + b * s1[1]
               = ----------------------- (in legacy code variables)
                  a * s0[0] + b * s0[1]

    In the un-regularized case, the above formula might be approximate.
    So we should avoid using that if possible.

   */

  num = lr->t1[0] + lr->t1[1] + lr->tb * (lr->jt - lr->js);
  den = lr->t1[0] * ip - lr->t1[1] * im;

  /* Note that,
  
      t1[0] = sum_j (j + 0.5 - js) <dE^2>[j] >= 0
  
    and 

      t1[1] = sum_j (j + 0.5 - js) <dE^2>[j] <= 0,
      
    so den >= 0 */

  if (fabs(den) < min_e2sum) {
    fallback_activated = 1;
    goto FALLBACK;
  }

  del = num / den;

  if ((cm = 1.0 - del*ip) < 0.0) {
    cm = 0.0;
  }

  if ((cp = 1.0 + del*im) < 0.0) {
    cp = 0.0;
  }

  if (fabs(cm + cp) < min_csum) {
    fallback_activated = 1;
    goto FALLBACK;
  }

#ifdef MB_DBG__
  fprintf(stderr, "cm %g, cp %g, del %g = %g/%g num = %g+%g\n",
    cm, cp, del, num, den, t1[0]+t1[1], tb*(jt-js));
#endif

  // normalize cm and cp such that they sum to 1.0
  // However, cm and cp are NOT to be confused with
  // am (a-) and ap (a+)
  {
    double tmp = 1.0/(cm + cp);
    cm *= tmp;
    cp *= tmp;
  }

  if (cm < lr->frac_min) {
    cp = 1.0 - (cm = lr->frac_min);
  } else if (cp < lr->frac_min) {
    cm = 1.0 - (cp = lr->frac_min);
  } else {
    lr->quality = 1;
  }

FALLBACK:

  {
    double renorm_num, renorm_den;

    //renorm_num = cm * lr->s1[0] + cp * lr->s1[1];
    renorm_den = cm * lr->s0[0] + cp * lr->s0[1];

    // even if den >= min_size (say 0), it is still possible
    // for renorm_den to be zero.
    // For example:
    // e.g., cm*s0[0] + cp*s0[1] = 1*0 + 0*3
    if (fabs(renorm_den) < min_size) {
      fallback_activated = 1;
    }

    if (fallback_activated) {
      // use the plain average for the fallback case
      cm = cp = 1.0;
      lr->quality = 0;
    }

    lr->imbalance = (cm - cp) / (cm + cp);
    lr->success = 1;

    // we need to recompute renorm_den as the cm and cp
    // may have been changed in the fallback case
    renorm_num = cm * lr->s1[0] + cp * lr->s1[1];
    renorm_den = cm * lr->s0[0] + cp * lr->s0[1];

    est_value = renorm_num / renorm_den;
  }

  /*
  if (lr->type == MB_IIE_LR__TYPE_GRID) {
    printf("balance %g: s0 %g, %g, s1 %g, %g, c %g, %g\n",
        est_value,
        lr->s0[0], lr->s0[1],
        lr->s1[0], lr->s1[1],
        cm, cp);
  }
  */

  return est_value;
}



#endif
