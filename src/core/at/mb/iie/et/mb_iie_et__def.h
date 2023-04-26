#ifndef AT__MB_IIE_ET__DEF_H__
#define AT__MB_IIE_ET__DEF_H__

typedef struct mb_t_ mb_t;



typedef struct mb_iie_et_item_cache_t_ {

  double value;

  /* if the tags are the same, it means the raw data
   * the integral identity draw from have not changed
   *
   * we can set it to be total number of visits to
   * the window accumulator.
   * */
  double tag;

  double expires;

} mb_iie_et_item_cache_t;


typedef struct mb_iie_et_item_t_ {

  double value; /* the value of et */

  double imbalance;

  int quality; /* the current value is of good quality */

  mb_iie_et_item_cache_t cache[1];

} mb_iie_et_item_t;


typedef struct mb_iie_et_cache_params_t_ {
  int enabled;
  double lifespan;
} mb_iie_et_cache_params_t;


typedef struct mb_iie_et_t_ {
  int n; /* same as mb->n */
  mb_iie_et_item_t *items;
  mb_iie_et_cache_params_t cache_params[1];
} mb_iie_et_t;



#endif
