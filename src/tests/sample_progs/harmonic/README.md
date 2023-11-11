# Harmonic oscillator

For testing the distribution bias.

$$V(x) = \frac{1}{2} \, x^2 + \frac{T - T_\mathrm{min}}{T_\mathrm{max} - T_\mathrm{min}} c \, x.$$

With $c = 0$, $V(x) = \frac{1}{2} x^2$, $U = \frac{1}{2\beta}$.

With $c = 1$, the $x$ distribution at $T = T_\mathrm{max}$ is given by
$$
p(x) = \frac{1}{\sqrt{2\pi T_\mathrm{max}}}\exp\left[-\frac{(x + 1)^2}{2 T_\mathrm{max}}\right].$$

## Testing case

The high temperature bias is a linear term.

Running the program:

```sh
make at.exe && ./at.exe
```

Plotting the result

```gnuplot
plot [-2:2] "xhist_Tref.dat" u 1:3 w l, "xhist_Tmax.dat" u 1:3 w l, exp(-x**2/0.2)/(0.2*pi)**0.5, exp(-(x+1)**2/0.4)/(0.4*pi)**0.5
```

The reference temperature simulation is a symmetric distribution, while the high temperature is biased toward the negative axis.

The resulting distributions match their analytical solutions.
