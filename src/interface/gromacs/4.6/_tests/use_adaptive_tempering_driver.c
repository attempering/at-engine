#include "../src/kernel/adaptive_tempering_driver.h"

int main(void)
{
  llong_t step = 0, nsteps = 500000;

  at_t* at = at_open("at.cfg", FALSE, TRUE, 1.0, 0.01, 0);

  at->Ea = 0.0;

  for (step = 1; step <= nsteps; step++) {
    at_move(at, step, (step == 1), (step == nsteps), TRUE, FALSE);
  }

  at_close(at);

  return 0;
}
