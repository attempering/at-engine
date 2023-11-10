#include "at-engine__src.h"

#include "veritools/models/double_well/double_well.h"
#include "veritools/utilities/histogram/histogram.h"


long nsteps = 1000000;

double temp = 0.1;
double sigma = 0.2;


static void run()
{
  long t;
  double beta = 1.0/temp;
  double_well_t dw[1];
  histogram_t hist[1];

  double_well__init(dw, NULL);

  histogram__init(hist, -5, 5, 0.01);

  for (t = 1; t <= nsteps; t++) {
    double_well__metro(dw, beta, sigma);
    histogram__add(hist, dw->x);
  }

  printf("acc. ratio: %g\n", double_well__get_acc_ratio(dw));

  histogram__save(hist, "xhist.dat");
  histogram__finish(hist);

  double_well__finish(dw);
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
