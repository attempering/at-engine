#ifndef AT__MB_IIE_ET__DEF_H__
#define AT__MB_IIE_ET__DEF_H__

typedef struct at_mb_t_ at_mb_t;



typedef struct at_mb_iie_et_item_cache_t_ {

  double value;

  /* if the tags are the same, it means the raw data
   * the integral identity draw from have not changed
   *
   * we can set it to be total number of visits to
   * the window accumulator.
   * */
  double tag;

  double expires;

} at_mb_iie_et_item_cache_t;


typedef struct at_mb_iie_et_item_t_ {

  double value; /* the value of et */

  double imbalance;

  int quality; /* the current value is of good quality */

  at_mb_iie_et_item_cache_t cache[1];

} at_mb_iie_et_item_t;


typedef struct at_mb_iie_et_cache_params_t_ {
  int enabled;
  double lifespan;
} at_mb_iie_et_cache_params_t;


typedef struct at_mb_iie_et_t_ {
  int n; /* same as mb->n */
  at_mb_iie_et_item_t *items;
  at_mb_iie_et_cache_params_t cache_params[1];
} at_mb_iie_et_t;



#endif
