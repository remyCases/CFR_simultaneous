# CFR Simultaneous

Attempt to work through An Introduction to Counterfactual Regret Minimization of Todd W. Neller and Marc Lanctot.

## Compilation

```shell
make build
```

Exe can be found in /build/bin

## Execution

### Examples

To run examples of CRF using default values

#### RPS

```shell
make run_rps
```

```shell
make run_blt
```

### More general case

```shell
.\build\bin\main.exe [TYPE] [N] 
```

Where TYPE is either `RPS` or `Blotto` and `N` the number of MC steps.
