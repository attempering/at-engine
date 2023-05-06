# Unit tests

Unit test programs test features and functionalities.

Except the [common](common), every directory contains unit tests to a specific core module.

The [common](common) directory contains a common Makefile and symbolic links
to the core files and external packages.
The simplest way to build a new unit test case
is to add a second-level directory,
such as `langevin/move` or `mb/iie`,
and symbolically link the common Makefile.
