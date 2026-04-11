# CPU Scheduler Simulator

This project implements a simple CPU scheduling simulator with:

- FCFS (First Come First Serve)
- SJF (Shortest Job First, non-preemptive)

The simulator reads a workload file and prints:

- timeline of which process runs each time unit
- start time and completion time for each process
- waiting time, turnaround time, and response time

## Build

### With WSL / Linux toolchain

```bash
cd /mnt/c/Users/lamee/OneDrive/Documents/KFUPM/senior/25B/os/project/cpu-scheduler-simulator
gcc -Wall -Wextra -std=c99 src/main.c src/parser.c src/simulator.c src/output.c src/metrics.c -o scheduler
```

### With Make

If `make` is available, run:

```bash
cd /mnt/c/Users/lamee/OneDrive/Documents/KFUPM/senior/25B/os/project/cpu-scheduler-simulator
make
```

## Run

Use the default workload file:

```bash
./scheduler
```

Or specify a different workload file:

```bash
./scheduler inputs/workload2.txt
```

## Notes

- `main.c` now accepts an optional workload filename argument.
- The default file is `inputs/workload1.txt`.
