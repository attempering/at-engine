#ifndef AT_BIAS__DEF_H__
#define AT_BIAS__DEF_H__

/* high-temperature bias

  H = kappa* H0 + epsilon * H1
  kappa = 1-(T-Tref)*(1-kappa0)/(Tmax-Tref) if T>Tref; kappa=1 if T<Tref
  epsilon= epsilon0*(T-Tref)/(Tmax-Tref) if T>Tref; epsilon=0 if T<Tref

*/
typedef struct at_bias_t_
{
  int       bTH;
  double    TH_Tref;
  double    *kappa, *epsilon;
  double    kappa0, epsilon0;
} at_bias_t;

#endif
