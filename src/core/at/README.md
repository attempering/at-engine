# at

Adaptive tempering core files

## Usage

The usage example is included [../_tests].

### As a header-only package

```C
#include "at__src.h"

int main()
{
  ...
}
```

```sh
gcc myprog.c
```

### Conventional header/source use

```C
#include "at.h"

int main()
{
  ...
}
```

Compile this file as

```sh
gcc -Ipath/to/at myprog.c at__src.c
```

## Sub Modules

* [context](context): definitions that allows switching between the ANSI and GROMACS context
* [utils](utils): common utilities
* [sm](sm):  simple accumulator
* [mb](mb): multiple-bin estimators
* [eh](eh): energy histogram
* [langevin](langevin): Langevin equation

* [zcom](zcom): common utilities
