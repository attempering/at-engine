# Using the AT-Engine library

## Usage

The at-engine library can be used a head-only library.

Several usage examples are included [sample programs](../src/sample_progs).

### As a header-only package

```C
#include "at-engine__src.h"

int main()
{
  at_t* at = at__open("at.cfg", NULL, 0);

  for (step = 1; step <= nsteps; step++) {
    energy = mc_step(your_system, at->beta);
    at__step(at, energy, step, NULL);
  }

  at__close(at);

  return 0;
}
```

```sh
gcc myprog.c -lm
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
gcc -Ipath/to/at-engine-core-dir myprog.c at-engine__src.c -lm
```
