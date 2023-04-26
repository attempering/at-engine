#ifndef AT__MB_IIE_LR__DEF_H__
#define AT__MB_IIE_LR__DEF_H__


enum {
  MB_IIE_LR__TYPE_BIN  = 0,
  MB_IIE_LR__TYPE_GRID = 1,
};


/**
 * Note on the window division convention.
 *
 * The window division convention applies to
 * the routine mb_iie_et__calc_et() only.
 * Basically, there are two alternative ways of
 * defining the auxiliary function phi(beta).
 * The convention concerns the dividing point
 * of the smooth part of phi(beta).
 *
 * In the convention adopted by the original paper,
 * the division point is set at the right boundary
 * of the central bin, that is
 *
 *    beta* = beta[ib+1].
 *
 * In the convention adopted by the legacy code,
 * the division point is set at the left boundary.
 *
 *    beta* = beta[ib].
 *
 * In both cases,
 *
 *    phi(beta) = phi_s(beta) + phi_t(beta)
 *
 * But the discontinuity of phi_s(beta) or phi_t(beta)
 * occurs at different places.
 * Here,
 *
 *  o As a whole, the function phi(beta) is a continuous function
 *    that starts from zero and ends at zero.
 *
 *  o The component function phi_s(beta) is a piecewise-linear
 *    function that is discontinuous only at the division point
 *    beta* mentioned above. (The subscript "_s" stands for [s]mooth.)
 *    And it is defined
 *
 *                          beta - beta-
 *                   /  a- ---------------   (for beta < beta*)
 *                   |      beta* - beta-
 *    phi_s(beta) = <
 *                   |      beta - beta+
 *                   \  a+ ---------------   (for beta > beta*)
 *                          beta+ - beta*
 *
 *    At the discontinuity beta = beta*, phi_s(beta) drops
 *    from a- to -a+, which means it drops by 1.0 as
 *    a- + a+ = 1.
 * 
 *  o phi_t(beta) is a function that vanishes
 *    except within the bin ib.
 *
 *    Under the paper convention,
 *
 *                             beta - beta[ib]
 *      phi_t(beta) = - -----------------------,
 *                       beta[ib+1] - beta[ib]
 *
 *    which evaluates as -0.5 at the midpoint of the bin.
 *    This function is 0 at beta = beta[ib], but gradually
 *    drops to -1 at beta = beta[ib+1], then abruptly rises
 *    back to 0.0.
 *
 *    Under the legacy-code convention,
 *
 *                       beta[ib+1] - beta
 *      phi_t(beta) = + -----------------------,
 *                       beta[ib+1] - beta[ib]
 *
 *    which is 0.5 at the midpoint of the bin.
 *    This function starts at 0.0 at beta = beta[ib],
 *    rising to 1.0 at the point,
 *    and drops to 0.0 at beta = beta[ib+1].
 *
 *    The reason that phi_t has to be defined in this way
 *    is that it allows the discontinuities of phi_t and phi_s
 *    to cancel out at the same spot, that is, beta = beta*.
 *
 *    In the case of paper convention, phi_s has a singularity at
 *    beta* = beta[ib+1],
 *    at which point, phi_s drop from a- to -a+, which means that
 *    it is reduced by 1.0.
 *    phi_t decreases from 0 to -1.0 at left bin boundary of beta* = beta[ib+1],
 *    and then rises back to 0.0.
 *    which means phi_t is increased by 1.0 at beta*. 
 *    Thus the discontinuity of phi_s and phi_t cancel out at beta*.
 * 
 *    In the case of legacy-code convention, the singularity of phi_s
 *    is located at beta* = beta[ib], at which point, phi_s drops by 1.0.
 *    And the corresponding phi_s encounters a sudden increase by 1.0 at beta*.
 *    So again the two discontinuities cancel out.
 *
 */

enum {
  MB_IIE_LR__WIN_DIV_LEGACY = 0,
  MB_IIE_LR__WIN_DIV_PAPER = 1,
  MB_IIE_LR__WIN_DIV_DEFAULT = MB_IIE_LR__WIN_DIV_PAPER,
};


#define MB_IIE_LR__DEFAULT_MIN_SIZE 1e-3


typedef struct mb_t_ mb_t;
typedef struct sm_t_ sm_t;
typedef struct mb_iie_zerofiller_t_ mb_iie_zerofiller_t;



/* left-right integral-identity estimator */
typedef struct mb_iie_lr_t_ {

  /* global input parameters */
  double frac_min; /* minimum acceptable coefficient during left/right combination */
  double cv_shift_max; /* maximum fraction for shifting energy fluctuations for the simple estimator of CV */

  double min_size; /* minimum number of sample size to use the I.I. */

  /* case-by-case input parameters */
  int type; /* whether it is for a bin value or a grid value */
  int win_div; /* convention for the window division */
  int ib; /* index of the bin */

  int jb; /* index of the window division
           * in the legacy-code convention, jb = ib
           * in the paper convention, jb = ib + 1 */

  int js; /* index of the window left boundary */
  int jt; /* index of the window central boundary */

  sm_t *sm0; /* origin of the moment accumulators
              * which is either mb->accum->sums for plain accumulators
              * or mb->accum->winaccum->items[ib].sums - .js
              * for window accumulators */

  /* reference to the window accumulator item
   * or NULL if the plain accumulators are used */
  mb_accum_winaccum_item_t *winaccum_item;

  /* temporary data */
  double s0[2];  /* zeroth-order moments
                  * counts from the left and right windows,
                  * in the regularized case, they should be simply 
                  * to the left and right window widths
                  * i.e., s0[0] = jb - js, s0[1] = jt - jb */

  double s1[2];  /* first-order moments for the average energy
                  * window sums from the left and right windows */

  double t1[2];  /* first-order moments for the energy fluctuation
                  * phi multiplied higher-order moments,
                  * used to compute the coefficients
                  * of linear combination */

  double tb;     /* first-order moment for central bin
                  * correction moment for the bin ib, this terms exists
                  * only for a bin-averaged quantity.
                  * For bin-boundary quantities, set it to zero. */

  /* output results */
  double imbalance; /* difference between the left and right coefficients */
  int quality; /* the result is good */
  int success; /* the function returns */

  mb_t *mb;
  mb_iie_zerofiller_t *zerofiller;

} mb_iie_lr_t;



#endif

