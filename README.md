# AT-Engine

## Overview

This package implements an adaptive tempering algorithm,
which is a single-copy simulated tempering method
based on a continuous temperature range.
The algorithm adopts a few adaptive techniques
to progressively refine the parameters related to
the partition function, which serves as the weights
to the overall temperature distribution.

The current code is written in ANSI C.

## Getting started

A typical simulation goes like this:

```C
at_t* at = at__open("at.cfg", NULL, AT__INIT_VERBOSE);

for (step = 1; step <= nsteps; step++) {
  energy = md_or_mc_step(your_system, at->beta);
  at__step(at, energy, step, NULL);
}

at__close(at);
```

Simulation parameters are provided in the configuration file `at.cfg`.

## Options

A brief manual on the configuration file `at.cfg`
is included under the [doc](doc) directory.

## New features

In comparison with the 2010 paper [1], the current code brings about
a few key improvements on the overall accuracy and performance.

* Metropolisation.  The Langevin equation used in the original
algorithm is approximate, and may be inaccurate for an overly
large step size.
In the new code, we deploy a Metropolisation technique, which employs
an acceptance step to overcome the approximation,
and hence allows a much larger time step.

* Moderation on the temperature diffusion.  Initial stages of the
simulation often pose stability issues because of lack of data to
guide an efficient diffusion in the temperature space.  A few
parameters `langevin_no_skip`, `langevin_bin_min_visits`
are now added to prevent unreasonably large temperature transitions
in early stages.

* Zero-filling mechanism. The integral identity draws estimates from
a window of bins. Sometimes the bin range contains missing data
that needs to be filled in order for the identity to work proper.
The zero-filling mechanism is now added to fill the missing values
in the range of concern.
This mechanism is also employed in the Metropolisation step for
the Langevin equation in the above to compute the integral of
the average potential energy over a beta range.

* Customizable temperature distribution. The temperature distribution
can now be customized with a linear combination of Gaussian distributions
or the flat distribution with variable weights.

## References

[1] C. Zhang, J. Ma, J. Chem. Phys. 132, 244101 (2010)
[https://doi.org/10.1063/1.3435332](https://doi.org/10.1063/1.3435332)
