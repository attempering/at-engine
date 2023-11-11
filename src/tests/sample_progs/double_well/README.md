# Double-well system

Testing the bias potential functionality.

The base Hamiltonian is a symmetric double well.
The bias potential is a linear potential.

```sh
make at.exe && ./at.exe
```

Plotting the resulting distributions:

```gnuplot
plot [-2:2] "xhist_Tref.dat" u 1:3 w l, "xhist_Tmax.dat" u 1:3 w l
```

The reference temperature distribution is symmetric,
while the high temperature is biased.
