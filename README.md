# Kernel files for the legacy PCST code

* [PCST kernel files](src/interface/gromacs/4.6/src/kernel/adaptive_tempering)
* [Test case: Gaussian energy model](src/sample_progs/gaussian)

The PCST algorithm is the main engine that drives
the temperature transitions in the multiple-canonical ensemble.

Things to know

* Kernel files have been refactored in favor of a modular design.

* Kernel files is a header-only library written in C.
  The `xxx.h` files are the declarations,
  the `xxx__src.h` files are implementations.
  The latter can been included as header files,
  or compiled as .c files

* Kernel files can be compiled with an ANSI C compiler
  without dependencies on external libraries.
  They can also be compiled along with the rest of the GROMACS
  code.
  The version of the original hosting version of GROMACS
  for the legacy PCST code is 4.6.3, in which the main compiler
  is a C compiler instead of a C++ compiler.

* Sample test cases using the API offered by the kernel files
  are collected in the [sample](sample) directory.

* A brief manual on the configuration file `at.cfg`
  is included under the [doc](doc) directory.
