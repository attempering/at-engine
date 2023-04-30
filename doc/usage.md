# Using the AT-Engine library

## Usage

The at-engine library can be used a head-only library.

Several usage examples are included [sample programs](../src/sample_progs).

### As a header-only package

```C
#include "at-engine__src.h"

int main()
{
  at_t* at = at__open("at.cfg", AT__FALSE, AT__TRUE, NULL, AT__FALSE);

  for (step = 1; step <= nsteps; step++) {
    at->energy = ...;
    at__step(at, step, NULL);
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
gcc -Ipath/to/core myprog.c at-engine__src.c -lm
```
