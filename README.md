# CFR Simultaneous

Attempt to work through An Introduction to Counterfactual Regret Minimization of Todd W. Neller and Marc Lanctot.

# Compile code

```shell
make build
```

Exe can be found in /build/bin

# Run code

```shell
.\build\bin\main.exe TYPE N 
```

Where TYPE is either `RPS` or `Blotto` and N the number of MC steps.

# Known issues

Blotto resolution seems to not converge to a correct solution.