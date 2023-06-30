# Configuration files for at-engine programs

## Disclaimer

The document is produced in the hope to be useful
for guiding the use of the at-engine code.
However, there is no explicit or implied warranty of
merchantability or fitness for any particular purpose.

The document is currently under construction and
subject to future changes.

The information contained in this document may not be accurate
or up to date.

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
the source code for the true meaning of various options.

2023.4.26

## I. Overview

### The configuration file

The configuration file, usually named `at.cfg`,
is a file that contains all configurable options
for running a simulation.

This configuration file share a similar format as the GROMACS
`.mdp` file.
However, the two are independent.

The configuration file is not compiled before simulation.
They are read and parsed at the beginning of the simulation by the program.

For continuation runs, it is important to ensure that the parameters
contained in the configuration file are unchanged.

### Basic format

The format of each option is

```cfg
key = value
```

You can freely add spaces before and after the equal sign "`=`".

The keys are case insensitive.
The dashes "`-`" and underscores "`_`"
are regarded as the same in the keys.
For example, `beta-min` and `Beta_Min`
mean the same thing.

For backward compatibility, we will give alias key names
from the legacy program.
For example,

  `mb-del-kT` (old: `mbest_delta_kT`)

means that `mbest_delta_kT` is the legacy key name for `mb-del-kT`,
both are mapped to the same variable.

The legacy keys are supported and may be removed in future releases.

### Summary of most important parameters

The following options are among the most important ones.

* `beta-min`, `beta-max`, `beta-del`
* `thermostat-temp` (old: `T0`)
* `ensemble-factor`, `ensemble-mode`
* `langevin-dt` (old: `Tdt`)

## II. Basic tempering parameters

### II.A. Temperature range and binning

Temperature range is specified in terms of
the reciprocal temperature `beta = 1/(k*T)`,
where `k` is the Boltzmann constant.

In the GROMACS units, `k = 0.0083145112`,
so `T = 300K` corresponds to a value of about `beta = 0.4009`,
and `T = 600K` a value of about `beta = 0.2045`.

* `beta-min`: minimal reciprocal temperature

  Variable: `at->distr->domain->beta-min`.

  Default: none.

* `beta-max`: maximal reciprocal temperature

  Variable: `at->distr->domain->beta-max`.

  Default: none.

* `beta-del`: bin width of the reciprocal temperature

  Conditions: > 1e-6.

  Variable: `at->distr->domain->beta-del`.

  Default: 0.0001.

#### Remarks on the temperature range

Internally, the temperature grid points are represented by
the reciprocal temperature `beta` instead of `T` or `k*T`.
Thus, it is common to choose values of `beta-min` and `beta-max`
to be decent multiples of `beta-del`, which can minimize
undesirable effects on round-off errors.

For example, if `beta-del = 0.0001`,
it is better to use values of `beta-min = 0.2`
and `beta-max = 0.41` instead of values of
`beta-min = 0.20045275` and `beta-max = 0.4009055`.

In principle the bin size should scale with the system size `n`
as `beta-del ~ 1/sqrt(n)`.
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

In the program, the overall temperature distribution density
is given by the product of two factors, `f` and `w`

`rho(beta) d beta = f(beta) w(beta) d beta`

#### II.C.1. `w(beta)`

The `w(beta)` factor is given by this equation:

`w(beta) = 1/beta^{ensemble-factor}`

* `ensemble-factor`: ensemble exponent for beta.
  d(beta) / beta^{ensemble-factor}

  Default: 1.0.

#### II.C.2. `f(beta)`

The `f(beta)` factor is a new feature from the PCST paper.

The `f(beta)` factor has three possibilities.

* Type 0. Flat distribution: `f(beta) = 1`
          This is the case that falls back to the classic
          adaptive tempering algorithm.

* Type 1. single Gaussian distribution:
          `f(beta) = exp[-(beta - beta0)^2/(2 sigma^2)]`
          This is the new distribution suggested by the PCST paper.

* Type 2. Exponential distribution:
          `f(beta) = exp(-c beta)`.
          This case is not documented, and only for used for testing.

* Type 3. Composite distribution.

Options:

* `ensemble-mode`: distribution type selector.

  * 0: flat.
  * 1: single Gaussian.
  * 2: exponential.
  * 3: composite distribution.

  Default: 0 (flat distribution).

* Type 1 options:

  $$w(\beta) = \exp[-(\beta - \beta_0)^2/(2 \sigma^2)]$$

  * `ensemble-beta0` for $\beta_0$.

    Default: `0.5 * (beta-min + beta-max)`.

  * `ensemble-sigma` for $\sigma$.

    Default: 1.0.

* Type 2 options:

  $$f(\beta) = \exp(-c \beta)$$

  * `ensemble-c` for $c$.

    Default: 0.0.

    Notes: The default value of 0.0 reproduces a flat distribution.

* Type 3 options:

  $$f(\beta) = \sum_{i=1}^n w_i f_i(\beta).$$

  * `ensemble-n-components` for $n$: number of components

  * `ensemble-component-1`, `ensemble-component-2`, ...

    ensemble component specifications.

    Each component specification follows the following format:

    `type,weight[:parameters]`

    * `type` gives the type of the component, which can be
      "`flat`" or "`gaussian`".

    * `weight` is $w_i$ in the equation.

    * If `type` is "`flat`", then no parameter is needed.

      For example:

      `ensemble-component-1=flat,1.2`

      means that the first component is a flat distribution
      with the relative weight being $1.2$, i.e.,

      $w_1 f_1(\beta) = 1.2$.

    * If `type` is "`gaussian`", the parameters contains
      two numbers for the center $\beta_0$ and $\sigma$
      respectively, separated by comma.

      For example:

      `ensemble-component-2=gaussian,0.7:0.75,0.05`

      means that the second component $f_2$ is a Gaussian distribution,
      with the relative weight $w_2 = 0.7$.
      The center $\beta_0$ and width $sigma$
      for the component is given by $0.75$ and $0.05$, respectively.
      That is,

      $w_2 f_2(\beta) = 0.7 \exp[-\frac{1}{2}((\beta - 0.75)/0.05)^2]$

### II.D. Bias potential settings

Bias potential settings apply where there is specifically assigned bias potential.

$$H = \kappa H_0 + \epsilon H_1$$

where

* $H_0$ is the original Hamiltonian function.
* $H_1$ is the bias potential.

* $\kappa = 1 - (T - T_{ref})*(1 - \kappa_0)/(T_{max} - T_{ref})$
  if $T > T_{ref}$
  or
  $\kappa = 1$ if $T < T_{ref}$

* $\epsilon = \epsilon_0 (T - T_{ref}) / (T_{max} - T_{ref})$
  if $T > T_{ref}$
  or $epsilon=0$ if $T < T_{ref}$

With the default values $\kappa_0 = 1, \epsilon_0 = 0$,
we have $\kappa = 1, \epsilon = 0$ no matter the temperature.
That means, $H = H_0$.

The options are given below:

* `boost-mode`: enable the bias potential

  * 0: disabled
  * 1: enabled

  Variable: `at->distr->bias->enabled`.

  Default: 0.

* `boost-Tref`: reference temperature

  Variable: `at->distr->bias->ref_temp`.

  Default: 300.0 (unit is K).

* `kappa0`: $\kappa_0$, magnitude of the original Hamiltonian

  Variable: `at->distr->bias->kappa0`.

  Default: 1.0.

* `epsilon0`: $\epsilon_0$, magnitude of the bias potential

  Variable: `at->distr->bias->epsilon0`.

  Default 0.0;

### II.E. Basic tempering parameters

* `nsttemp`: frequency of tempering.
  This value means that the program invokes the tempering function every this number of MD steps.

  For example: for an MD simulation of `10000` steps,
  and `nsttemp = 5`, then in `2000` steps out of the `10000` steps
  tempering would occur.

  Default: -1.

The actual tempering details are given by the Langevin equation settings.

### II.F. Langevin equation settings

The Langevin equation is the engine that drives the temperature transitions.

`d T = [ E - Et - (dlnwf/dbeta) ] dt + T sqrt(2*dt) dW`

* `Tdt`: time step for integrating the Langevin equation, `dt` in the above equation.

  Default: 1e-5.

* `dTmax`: threshold to clamp the maximum temperature change in a single application of the Langevin equation.

  Default: 25.0

* `move_repeats`: number of repeats in every tempering step.

  The Langevin equation for tempering will be repeated
    this number of times in a tempering step.

  Default: 1.

### II.G. Multiple-bin estimator settings

Multiple-bin estimator settings share the prefix of `mbest_`

#### II.G.1. Window construction settings

* `mb-use-single-bin`: use a single bin estimator instead of the multiple-bin estimator

  * 0: disabled
  * 1: enabled

  Default: 0.

  Remarks: if `mb-use-single-bin == 1`,
  then `mb-mbin-mode`, `mb-del-xxx` options are no longer active.

* `mb-mbin-mode` (`mbest_mbin_mode`): window width scaling mode.

  Available modes:

  * 0: d(beta) mode, meaning that the window size (in `Delta beta`) is independent of the `beta` value.

  * 1: dT/T = d(lnT) = |d(beta)/beta| mode, meaning that the window size (in `Delta beta`) is proportional to the `lnT` value.
  
  * 2: d(kT) mode, meaning that the window size (in `Delta beta`) is proportional to the value of `kT = 1/beta`.
  This means the averaging window at 600K is twice as large as the window at 300K.

  Default: 1

* `mb-del-xxx` (old: `mbest_delta_xxx`): window size.

  The following options are all mapped to a single variable `mb->bwdel`.

  * `mb-del-lnT` (old: `mbest_delta_lnT`): half window width in terms of `d(lnT)`,
      only applicable for `mb-mbin-mode == 1`.

    Conditions: `mb->bwdel > beta-del/beta-max`

    Default: 0.05 (in terms of percentage of T).

  * `mb-del-beta` (old: `mbest_delta_beta`): half window width in terms of `beta`,
      only applicable for `mb-mbin-mode == 0`.

    Conditions: `mb->bwdel > beta-del`.

    Default: 0.02 (in terms of beta).

  * `mb-del-kT` (old: `mbest_delta_kT`): half window width in terms of `kT`,
      only applicable for `mb-mbin-mode == 2`.

    Conditions: `mb->bwdel > beta-del/pow(m, 2.0)`

    Default: 0.1 (in terms of kT = 1.0/beta).

* `mb-use-sym-wins` (old: `mbest_sym_mbin`): force symmetric windows.

  Symmetric means being symmetric in the beta space.

  For example, a window for bin 100 would contains bins
  from bin 85 = 100 - 15 to 115 = 100 + 15.

  * 0: disabled, asymmetric.
  * 1: enabled, symmetric.

  Default: 1.

* `mb-use-single-bin` (`mbest_single_bin`): force the single-bin estimator.

  * 0: disabled.
  * 1: enabled.

  Default: 0.

#### II.G.2. Averaging behavior

* `mb-min-frac` (old `mbest_fracmin`): minimum acceptable combination for combining left/right estimators

  Default: 0.0.

* `mb-max-cv-shift` (old `mbest_cvshiftmax`): maximum fraction for shifting energy fluctuations

  Default: 1.0.

#### II.G.3. Adaptive averaging

The adaptive tempering algorithm uses average energy to establish runtime
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

  `shk = shk_base * shk_win_mul`.

The window multiplier `shk_win_mul` is inversely
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

    `shk-base = max{ min{ shrink-init, shrink-amp / t }, shrink-min }`

    where t is defined as the total number of visits divided by
    the number of temperature bins.

3. Mode 2 is a slight modification of mode 1, the formula is

    `shk-base = max{ min{ shrink-init, shrink-amp / t^{shrink-exp} }, shrink-min }`

    With shk_exp = 1.0, it reduces to the same formula used in mode 1.

Shrinking can be terminated when the total number of steps
exceeds `shrink-stop`.  When shrinking is stopped, the shrinking factor
is zero and `gamma` is unity.  So we recover the normal averaging
scheme.
.

* `mb-use-adaptive-averaging` (old: `mbest_damp`): turning on adaptive averaging or not

  * 0: disabled
  * 1: enabled.

  Conditions: 0 or 1.

  Variable: `mb->accum->winaccum->enabled`

  Default: 1.

* `shrink_mbin_adjust`: whether to apply the window adjustment for the shrinking factor

  * 0: disabled.
  * 1: enabled.

  Default: 1.

* `shrink_init`:

  Default: 0.01.

* `shrink_max`:

  Default: 0.01.

* `shrink_mode`:

  Default: 1.

* `shrinkmin`:

  Default: 0.0.

* `shrinkstop`:

  Default: -1.

* `shrinkamp`:

  Default: 0.1.

* `shrinkexp`:

  Default: 1.0.

#### II.G.4. Miscellaneous settings

* `mb-need-cv` (`mbest_needcv`): need estimators for the heat capacity

  * 0: disabled, no Cv
  * 1: enabled, Cv

  Default: 1

* `mb-verbose` (`mbest_verbose`): being verbose

  * 0: disabled
  * 1: enabled

  Default: 1.

* `mb-nst-refresh` (`nstrefresh`): frequency (in the number of MD steps) of recalculating Et for all temperatures

  Default: 10000.

#### II.G.5. Output

##### II.G.5.a. Full average file

* `mb-binary-file` (old:`mbav_file`): binary file name for the multiple-bin averages

  Default: "mb.dat" (old: "mb.av").

* `mb-text-file`: text file name for the multiple-bin averages

  Default: "mb-text.dat".

* `mb-nst-save` (old: `nstav`): frequency of writing averager files in number of simulation steps.

  The files written include `ze_file`, `rng_file`, `mbav_file`.

  Default: 10000.

  Remarks:

  * a negative or 0 value disables this feature.
  * the writing will occur in the final MD step no matter how large the value is.

* `mb-use-text-file`: load data from the text file.

  * 0: disabled, text.
  * 1: enabled, binary.

  Default: 1.

##### II.G.5.b. thermodynamic quantities file

The thermodynamic quantity file contains information about the average energy, number of visits, weighted sum, etc. of every bin

* `ze_file`: name of the file

  Default: "ze.dat" (legacy: "ZE")

## III. Basic output

### III.A. Trace file

The trace file outputs the current potential energy and temperature every few steps.

* `trace_file`: name of the trace file.

  Default: "trace.dat" (legacy: "TRACE").

  Remarks: by default, this file is always appended instead of rewritten.
  So if you restart a fresh simulation, make sure to delete the TRACE file.
  
* `nsttrace`: frequency of write the log file.
    A common value can be something like `1000`.

  * -1: disable the log file
  * 0:  writing the log file only in steps of outputting the .xtc file
  * any positive integer: write log file this number of steps.

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

  Default: "langevin-rng.dat" (legacy: "MTSEED").

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

  Default: 1.

* `ehist_keepedge` whether to truncated bins of zero number of visits on both sides of the energy histogram before writing them to file.

  * 0: disabled
  * 1: enabled

  Default: 0.

* `ehist_nozeroes` whether to write histogram entries
  when the number of visits to the bin is zero.

  * 0: disabled
  * 1: enabled

  Default: 0.

  Remarks: It is recommended to turn this option off
  `ehist_file` is to be used for gnuplot for plotting
  a two-dimensional diagram using the command `splot`.

#### IV.A.5. Energy histogram, reweighted histogram settings

* `ehist_mbin_file` file name of the reconstructed energy histogram.

  This file is always written in the text format.

  Default: "hist-recon.dat" (legacy file name "HMB").

* `ehist_skip` number of temperatures to skip over when writing the reconstructed energy histogram.

##### Energy histogram, reweighed histogram, beta distribution

Beta windowing methods

* `ehist_mbin_mode`

  * 0: beta mode
  * 1: lnT mode
  * 2: kT mode

  Conditions: 0, 1, or 2.
  
  Default: 1.

* `ehist_delta_xxx`

  * `ehist_delta_beta`

    Default: `0.02`.

  * `ehist_delta_lnT`

    Default: `0.05`.

  * `ehist_delta_kT`

    Default: `0.10`.
