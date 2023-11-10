#include "at-engine__src.h"

#include "veritools/utilities/rng/rng.h"
#include "veritools/utilities/histogram/histogram.h"


long nsteps = 1000000;

double temp = 0.1;
double sigma = 0.2;


static void run()
{
  long t;
  double x;
  rng_t rng[1];
  histogram_t hist[1];

  rng__init(rng, 0);
  x = sqrt(temp) * rng__rand_normal(rng);

  histogram__init(hist, -5, 5, 0.05);

  for (t = 1; t <= nsteps; t++) {
    x = sqrt(temp) * rng__rand_normal(rng);
    histogram__add(hist, x);
  }

  histogram__save(hist, "xhist.dat");
  histogram__finish(hist);

  rng__finish(rng);
}


static void do_args(int argc, char** argv)
{
  if (argc > 1) {
    temp = atof(argv[1]);
  }

  if (argc > 2) {
    sigma = atof(argv[2]);
  }
}

int main(int argc, char** argv)
{
  do_args(argc, argv);

  run();

  return 0;
}
