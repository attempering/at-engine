#ifndef AT_MB_SHK__DEF_H__
#define AT_MB_SHK__DEF_H__

/* shrinking rate of the adaptive averaging scheme */

/* The adaptive averaging scheme is based on this formula

    S  = S  * gamma + 1
    Sx = Sx * gamma + x

    <x> = Sx / S

  This scheme helps reducing the current data weight
  in the accumulator. 

  The data members in at_mb_shk_t help provide a consistent way
  of computing gamma for windows of different widths.
  The key quantity is the shrinking factor, which is
  represented by the variable name `shk` in the code.
  The shrinking factor is related to `gamma` as

    gamma = 1 - shk.

  With a constant shrinking factor or constant gamma,
  the total data size S will ultimately approach a
  constant saturation value of 1/shk.

  Thus, the quantity `shk` is gradually reduced in simulation.

  The basic formula for `shk` is given by

  shk = shk_base * shk_window_multiplier.
  
  The window multiplier (shk_window_multiplier) is inversely
  proportional to the window width (or the number of bins
  contained within) and the ensemble weight (which is the
  probability density of the overall beta distribution).
  In this way, larger windows that receive more
  visits will have a smaller shrinking factor so that
  the sample size in windows of different sizes can
  grow roughly uniformly.

  The same reasoning applies to the windows located at
  different locations of the temperature distribution.
  Windows centered at a temperature of larger ensemble weight
  will experience a smaller shrinking factor, so that
  their sample sizes will roughly in sync with windows
  centered at less populated temperature locations.

  There are three modes of calculating shk_base.

  1. In mode 0, the program uses a constant shrinking factor.
     Its value is equal to shk_min (whose default value is 0).
     This mode is used to disable adaptive averaging
     when shk_min is set to zero.

  2. In mode 1 (which is the default mode), the program uses the following formula
  
      shk_base = max{ min{ shk_max, shk_amp / t }, shk_min }

    where t is defined as the total number of visits divided by
    the number of temperature bins.

  3. Mode 2 is a slight modification of mode 1, the formula is

      shk_base = max{ min{ shk_max, shk_amp / t^{shk_exp} }, shk_min }

    With shk_exp = 1.0, it reduces to the same formula used in mode 1.

  Shrinking can be terminated when the total number of steps
  exceeds `shk_stop`.  When shrinking is stopped, the shrinking factor
  is zero and `gamma` is unity.  So we recover the normal averaging
  scheme.

 */

typedef struct at_mb_shk_t_ {
  int       mode;     /* 0: const, 1: amp/t, 2: amp/t^exp */

  double    base;     /* current generic shrink amplitude */

  int       window_adjusted;
                      /* adjust shrink rate according to beta window width */

  double    max;      /* initial and maximal shrink (adjusted) */

  double   *window_multiplier;
                      /* multipliers used to regularize the shrinking rate
                       * for windows of different sizes */

  double    min;      /* minimal value for enforcing acc. sampling */
  int       stop;     /* stop shrinking after this number of steps */
  double    amp;      /* amp t^(-exp) */
  double    exp;      /* amp t^(-exp) */

  int n;
} at_mb_shk_t;

#endif
