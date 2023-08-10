# Notes

## Cheap evaluation vs full evaluation (using the integral identity)

With the cheap evaluation, the bin average energy is estimated
from a single bin instead of the more expensive integral identity.

The bin average energy gives the deterministic or the force
part of the Langevin equation.

For the uncorrected Langevin equation,
we use the full evaluation to improve the accuracy of the force.
(However, even in this case, this may not be necessary given that
the Langevin equation is mainly determined by the random diffusion
instead of the deterministic force.)

Nonetheless, we will still use the full evaluation
for the uncorrected Langevin equation.

For the corrected Langevin equation, this routine is called twice
for every move. The first call is for suggesting an attempted move,
and the second is for the virtual reverse move in calculating
the Metropolisation correction.

If the time step is small, then the proposed move is always accepted.
And thus using the full evaluation may help a little.

If the time step is large, even the proposed move is not too precise,
it can be corrected by the Metropolisation step.
So it would be fine to use the cheap evaluation.
