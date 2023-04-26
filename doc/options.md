# at-engine configuration file

## Disclaimer

The document is produced in the hope that it will be useful
for guiding the use of the at-engine code.
However, there is no warranty or even implied one of
merchantability of fitness for any particular purpose.

The document is currently still under construction and subject to change.

The information contained in this document are sometimes
personal opinions and
cannot always be regarded as accurate or even definitive.
The reader is warned of the fact that the existence of possible
inaccuracies and errors in this document may lead the user to
improperly use the program.

Several issues for both the adaptive tempering code and the GROMACS code
it is based on have been discovered, and some have been documented.
It is the obligation of the end user of the program to research
and  apply these fixes before running a valid MD simulation.
This document, however, generally contains little information
on these issues.

Besides, there are other subtler ambiguities and pitfalls.
While we have tried to point out some the possible pitfalls,
it may be impossible to list all of them.

Due to limited time in preparing this document as well as
limited knowledge on the program, errors are inevitable.
For these, we must apologize and encourage the reader to explore
the source code for a correct meaning of various options.

2023.4.26

## I. Overview

### The configuration file

The configuration file, usually named `at.cfg`,
is a file that contains all configurable options
for running a simulation.

This configuration file share a similar format as the GROMACS
`.mdp` file.
However, the two are independent.

The configuration file are compiled before simulation.
They are read and parsed at the beginning of the simulation by the program.

For a multiple-run simulation, it is important to ensure that the parameters in the configuration file are unchanged.

### Summary of most important parameters

The following options are possibly among the most important ones.

* `beta_min`, `beta_max`, `beta_del`
* `T0`
* `ensemble_factor`, `ensemble_mode`
* `Tdt`

## II. Core parameters

### II.A. Temperature range and binning

Temperature range is specified in terms of
the reciprocal temperature `beta = 1/(k*T)` in GROMACS units,
where `k` is the Boltzmann constant

In the GROMACS units, `k = 0.0083145112`,
so `T = 300K` corresponds to a value of about `beta = 0.4009`,
and `T = 600K` a value of about `beta = 0.2045`.

* `beta_min`: minimal reciprocal temperature

  Variable: `at->bmin == at->mb->bmin`.

  Default: none.

* `beta_max`: maximal reciprocal temperature

  Variable: `at->bmax == at->mb->bmax`.

  Default: none.

* `beta_del`: bin width of the reciprocal temperature

  Conditions: > 1e-6.

  Default: 0.0001.

#### Remarks on the temperature range

Internally, the temperature grid points are represented by
the reciprocal temperature `beta` instead of `T` or `k*T`.
Thus, it is common to choose values of `beta_min` and `beta_max`
to be decent multiples of `beta_del`, which can minimize
undesirable effects on round-off errors.

For example, if `beta_del = 0.0001`,
it is better to use values of `beta_min = 0.2`
and `beta_max = 0.41` instead of values of
`beta_min = 0.20045275` and `beta_max = 0.4009055`.

In principle the bin size should scale with the system size `n`
as `beta_del ~ 1/sqrt(n)`.
A finer bin size is recommended for very large systems.

### II.B. Thermostat temperature

* `T0`: thermostat temperature for simulation.

  Default: 300 (unit is K).

  Remarks: the value should match the value(s) of `ref_t` in the `.mdp` file.

  In the code, temperature change of the system is realized
  through a technique of force scaling.
  This means that the actual temperature of the system is not
  changed, but the change is realized through scaling the force.

### II.C. Overall temperature distribution

In the program, the overall temperature distribution
is characterized by two factors, `f` and `w`

`rho(beta) d beta = f(beta) w(beta) d beta`

#### II.C.1. `w(beta)`

The `w(beta)` factor is given by this equation:

`w(beta) = 1/beta^{ensemble_factor}`

* `ensemble_factor`: ensemble exponent for beta.
  d(beta) / beta^{ensemble_factor}

  Variable: `mb->ens_exp`

  Default: 1.0.

#### II.C.2. `f(beta)`

The `f(beta)` factor is a new feature from the PCST paper.

The `f(beta)` factor has three possibilities.

* Type 0. Flat distribution: `f(beta) = 1`
          This is the case that falls back to the classic CST algorithm.

* Type 1. Gaussian distribution: `f(beta) = exp[-(beta - beta0)^2/(2 sigma^2)]`
          This is the new distribution suggested by the PCST paper.

* Type 2. Exponential distribution: `f(beta) = exp(-c beta)`.
          This case is not documented, and only for used for testing.

Options:

* `ensemble_mode`: distribution type selector.

  * 0: flat
  * 1: Gaussian, w(beta) ~ exp[-(beta - beta0)^2/(2 sigma^2)]
  * 2: exponential, w(beta) ~ exp(-c beta);

  Default: 0 (flat distribution).

* Type 1 options:

  * `ensemble_beta0`:

    Conditions: applicable only to `ensemble_mode == 1`

    Default: `0.5 * (mb->bmax + mb->bmin)`.

  * `ensemble_sigma`:

    Conditions: applicable only to `ensemble_mode == 1`

    Variable: `mb->invsigma2 = 1/(sigma*sigma)`

    Default: 1.0.

* Type 2 options:

  * `ensemble_c`: the value of c in the exponential beta distribution

    Conditions: applicable only to `ensemble_mode == 2`

    Variable: `mb->ensemble_c`

    Default: 0.0.

    Notes: The default value of 0.0 reproduces a flat distribution.

### II.D. Bias potential settings

Bias potential settings apply where there is specifially assigned bias potential.

`H = kappa * H0 + epsilon * H1`

where

* `H0` is the original Hamiltonian function.
* `H1` is the bias potential.

* `kappa = 1 - (T - Tref)*(1 - kappa0)/(Tmax - Tref)` if `T > Tref`
  or `kappa = 1` if `T < Tref`

* `epsilon = epsilon0 * (T - Tref) / (Tmax - Tref)` if `T > Tref`
  or `epsilon=0` if `T < Tref`

With the default values `kappa0 = 1, epsilon0 = 0`,
we have `kappa = 1, epsilon = 0` no matter the temperature.
That means, `H = H0`.

The options are given below:

* `boost_mode`: enable the bias potential

  * 0: disabled
  * 1: enabled

  Variable: `at->bTH`.

  Default: 0.

* `boost_Tref`: reference temperature

  Variable: `at->TH_Tref`.

  Default: 300.0 (unit is K).

* `kappa0`: kappa0, magnitude of the original Hamiltonian

  Variable: `at->kappa0`.

  Default: 1.0.

* `epsilon0`: epsilon0, magnitude of the bias potential

  Variable: `at->epsilon0`.

  Default 0.0;

### II.E. Basic tempering parameters

* `nsttemp`: frequency of tempering.
  This value means that the program invokes the tempering fucntion every this number of MD steps.

  For example: for an MD simulation of `10000` steps,
  and `nsttemp = 5`, then in `2000` steps out of the `10000` steps
  tempering would occur.

  Default: -1.

The actual tempering details are given by the Langevin equation settings.

### II.F. Langevin equation settings

The Langevin equation is the engine that drives the temperature transitions.

`d T = [ E - Et - (dlnwf/dbeta) ] dt + T sqrt(2*dt) dW`

* `Tdt`: time step for integrating the Langevin equation, `dt` in the above equation.

  Variable: `mb->lgv_dt`.

  Default: 1e-5.

* `dTmax`: threshold to clamp the maximum temperature change in a single application of the Langevin equation.

  Variable: `mb->lgv_dTmax`.

  Default: 25.0

* `move_repeats`: number of repeats in every tempering step.

  The Langevin equation for tempering will be repeated
    this number of times in a tempering step.

  Variable: `at->mvreps`.

  Default: 1.

### II.G. Multiple-bin estimator settings

Multiple-bin estimator settings share the prefix of `mbest_`

* `mbest_order`: order (deprecated, can be missing)

  Conditions: must be 1.

  Variable: `mb->order`.

  Default: 1.

#### II.G.1. Window construction settings

* `mbest_single_bin`: use a single bin estimator instead of the multiple-bin estimator

  * 0: disabled
  * 1: enabled

  Variable: `mb->flags & MB_ONEBIN`

  Default: 0.

  Remarks: if `mbest_single_bin == 1`,
  then `mbest_mbin_mode`, `mbest_delta_xxx` options are no longer active.

* `mbest_mbin_mode`: window width scaling mode.

  Available modes:

  * 0: d(beta) mode, meaning that the window size (in `Delta beta`) is independent of the `beta` value.

  * 1: dT/T = d(lnT) = |d(beta)/beta| mode, meaning that the window size (in `Delta beta`) is proportional to the `lnT` value.
  
  * 2: d(kT) mode, meaning that the window size (in `Delta beta` is proportional to the value of `kT = 1/beta`.
  This means the averaging window at 600K is twice as large as the window at 300K.

  Variable: `mb->bwmod`

  Default: 1

* `mbest_delta_xxx`: window size.

  The following options are all mapped to a single variable `mb->bwdel`.

  Variable: `mb->bwdel`.

  * `mbest_delta_lnT`: half window width in terms of `d(lnT)`,
      only applicable for `mbest_mbin_mode == 1`.

    Conditions: `mb->bwdel > mb->bdel/mb->bmin`

    Default: 0.05 (in terms of percentage of T).

  * `mbest_delta_beta`: half window width in terms of `beta`,
      only applicable for `mbest_mbin_mode == 0`.

    Conditions: `mb->bwdel > mb->del`.

    Default: 0.02 (in terms of beta).

  * `mbest_delta_kT`: half window width in terms of `kT`,
      only applicable for `mbest_mbin_mode == 2`.

    Conditions: `mb->bwdel > mb->del/pow(mb->bmin, 2.0)`

    Default: 0.1 (in terms of kT = 1.0/beta).

* `mbest_sym_mbin`: force symmetric windows.

  Symmetric means being symmetric in the beta space.

  For example, a window for bin 100 would contains bins
  from bin 85 = 100 - 15 to 115 = 100 + 15.

  * 0: disabled, asymmetric.
  * 1: enabled, symmetric.

  Variable: `mb->flags & MB_SYMWIN`.

  Default: 1.

#### II.G.2. Averaging behavior

* `mbest_regularize`: average behavior

  * 0: average is weighted by the counts in the bins
  * 2: average within a bin first

  Default: 2.

* `mbest_fracmin`: minimum acceptable combination for combining left/right estimators

  Default: 0.0.

* `mbest_cvshiftmax`: maximum fraction for shifting energy fluctuations

  Default: 1.0.

#### II.G.3. Adaptive averaging

The CST algorithm uses average energy to establish runtime
estimates of the partition function.
Such estimates serve as the weights of the temperature ensemble
and determine how many visits a certain temperature would receive.

Adaptive averaging scheme help damp out old and possibly incorrect data that
may hamper a smooth flow in the temperature space.

The adaptive averaging scheme is based on this formula

  $$\begin{aligned}
    S                 &= S   \gamma + 1, \\
    S_x               &= S_x \gamma + x, \\
    \langle x \rangle &= S_x / S.
  \end{aligned}$$

This scheme helps reducing the current data weight
in the accumulator.

The adaptive averaging module provides a consistent way
of computing gamma for windows of different widths.
The key quantity is the shrinking factor, which is
represented by the variable name `shk` in the code.
The shrinking factor is related to $\gamma$ as

  `gamma = 1 - shk`.

With a constant shrinking factor or constant gamma,
the total data size S will ultimately approach a
constant saturation value of 1/shk.

Thus, the quantity `shk` is gradually reduced in simulation.

The basic formula for `shk` is given by

  `shk = shk_base * shk_window_multiplier`.

The window multiplier `shk_window_multiplier` is inversely
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

    `shk_base = max{ min{ shrink0, shrinkamp / t }, shrinkmin }`

    where t is defined as the total number of visits divided by
    the number of temperature bins.

3. Mode 2 is a slight modification of mode 1, the formula is

    `shk_base = max{ min{ shrink0, shrinkamp / t^{shrinkexp} }, shrinkmin }`

    With shk_exp = 1.0, it reduces to the same formula used in mode 1.

Shrinking can be terminated when the total number of steps
exceeds `shrinkstop`.  When shrinking is stopped, the shrinking factor
is zero and `gamma` is unity.  So we recover the normal averaging
scheme.
.

* `mbest_damp`: turning on adaptive averaging or not

  * 0: disabled
  * 1: enabled.

  Conditions: 0 or 1.

  Variable: `mb->flags & MB_DAMP`

  Default: 1.

* `shrink_mbin_adjust`: whether to apply the window adjustment for the shrinking factor

  * 0: disabled.
  * 1: enabled.

  Variable: `mb->shk_winadj`.

  Default: 1.

* `shrink0`:

  Variable: `mb->shk_max`.

  Default: 0.01.

* `shrink_mode`:

  Variable: `mb->shk_mode`.

  Default: 1.

* `shrinkmin`:

  Variable: `mb->shk_min`.

  Default: 0.0.

* `shrinkstop`:

  Variable: `mb->shk_stop`.

  Default: -1.

* `shrinkamp`:

  Variable: `mb->shk_amp`.

  Default: 0.1.

* `shrinkexp`:

  Variable: `mb->shk_exp`.

  Default: 1.0.

#### II.G.4. Miscellaneous settings

* `mbest_needcv`: need estimators for the heat capacity

  * 0: disabled, no Cv
  * 1: enabled, Cv

  Variable: `mb->flags & MB_CV`.

  Default: 1

* `mbest_sbcorr`: include energy fluctuation correction due to a small bin width for the internal energy

  * 0: disabled
  * 1: enabled

  Variable: `mb->flags & MB_SBCORR`.

  Default: 1

* `mbest_verbose`: being verbose

  * 0: disabled
  * 1: enabled

  Variable: `mb->flags & MB_VERBOSE`.

  Default: 1.

* `nstrefresh`: frequency (in the number of MD steps) of recalculating Et for all temperatures

  Default: 10000.

#### II.G.5. Output

##### II.G.5.a. Full average file

* `mbav_file`: file name for the multiple-bin average files

  Default: "mb.av"

* `nstav`: frequency of writing averager files.

  The files written include `ze_file`, `rng_file`, `mbav_file`.

  Variable: `mb->av_nstsave`.

  Default: 10000.

  Remarks:

  * a negative or 0 value disables this feature.
  * the writing will occur in the final MD step no matter how large the value is.

* `mbav_binary`: output using binary format

  * 0: disabled, text.
  * 1: enabled, binary.

  Default: 1.

  Remark: this option is deprecated: a binary file is always output even when you demand a text one.

##### II.G.5.b. thermodynamic quantities file

The thermodynamic quantity file contains information about the average energy, number of visits, weighted sum, etc. of every bin

* `ze_file`: name of the file

  Default: "ze.dat" (legacy: "ZE")

* `mbest_wze_reps`: number of iterations before writing the ZE file.

  Default: 5

## III. Basic output

### III.A. Trace file

The trace file outputs the current potential energy and temperature every few steps.

* `trace_file`: name of the trace file.

  Variable: `at->trace_file`.

  Default: "trace.dat" (legacy: "TRACE").

  Remarks: by default, this file is always appended instead of rewritten.
  So if you restart a fresh simulation, make sure to delete the TRACE file.
  
* `nsttrace`: frequency of write the trace file.
    A common value can be something like `1000`.

  * -1: disable the trace file
  * 0:  writing the trace file only in steps of writing the .xtc file
  * any positive integer: write trace file this number of steps.

  Variable: `at->nsttrace`.

  Default: -1 (disabled).

### III.B. Random number state file

The adaptive tempering algorithm internally uses
a pseudorandom number generator
that is independent of the GROMACS one.

The RNG is called the Mersenne twister 19937,
and its state contains about 640 integers.

When resuming a previous simulation the random number state file
will be reloaded.

* `rng_file`: name of the random number state file.

  Default: "rng.dat" (legacy: "MTSEED").

## IV. Additional modules

### IV.A. Energy histogram

Below, `energy` means the potential energy.

The programs provides functionality of writing a two-dimensional
temperature-energy histograms.

The following set of options apply to behaviors to this functionality.

#### IV.A.1. Energy histogram global switch

* `ehist_mode`: energy histogram mode

  * 0: disabled.
  * 1: simple histogram.

  Conditions: 0 or 1.

  Default: 0.

#### IV.A.2. Energy histogram output settings

* `ehist_file` file name of the full energy histogram.

  Default: "hist.bin".

* `nsthist` frequency of writing energy histogram data to file

  Variable: `mb->eh_nstsave`.

  Default: 100000.

  Remarks: a negative or 0 value disable this feature.

* `ehist_binary` whether to use binary format for the full histogram data.

  * 0: disabled, text.
  * 1: enabled, binary.

  Default: 1.

#### IV.A.3. Energy histogram energy range

* `ehist_min` minimum potential energy

  Default: -12.6e4.

* `ehist_max` maximum potential energy

  Default: -9.0e4.

* `ehist_del` bin size of the potential energy histograms

  Default: 20.0 (GROMACS units).

#### IV.A.4. Energy histogram options

* `ehist_addahalf` whether to add half of delta E to represent every energy bin.

  * 0: disabled
  * 1: enabled

  Variable: `mb->flags & MB_AT_EH_ADDAHALF`

  Default: 1.

* `ehist_keepedge` whether to truncated bins of zero number of visits on both sides of the energy histogram before writing them to file.

  * 0: disabled
  * 1: enabled

  Variable: `mb->flags & MB_AT_EH_KEEPEDGE`

  Default: 0.

* `ehist_nozeroes` whether to write histogram entries
  when the number of visits to the bin is zero.

  * 0: disabled
  * 1: enabled

  Variable: `mb->flags & MB_AT_EH_NOZEROES`

  Default: 0.

  Remarks: It is recommended to turn this option off
  `ehist_file` is to be used for gnuplot for plotting
  a two-dimensional diagram using the command `splot`.

#### IV.A.5. Energy histogram, reweighted histogram settings

* `ehist_mbin_file` file name of the reconstructed energy histogram.

  This file is always written in the text format.

  Variable: `mb->eh_rfile`.

  Default: (legacy file name "HMB").

* `ehist_skip` number of temperatures to skip over when writing the reconstructed energy histogram.

  Variable: `mb->eh_skip`.

##### Energy histogram, reweighed histogram, beta distribution

Beta windowing methods

* `ehist_mbin_mode`

  * 0: beta mode
  * 1: lnT mode
  * 2: kT mode

  Variable: `mb->eh_bwmod`.

  Conditions: 0, 1, or 2.
  
  Default: 1.

* `ehist_delta_xxx`

  Variable: `mb->eh_bwdel`.

  * `ehist_delta_beta`

    Default: `0.02`.

  * `ehist_delta_lnT`

    Default: `0.05`.

  * `ehist_delta_kT`

    Default: `0.10`.
