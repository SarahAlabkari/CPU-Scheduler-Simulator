# CPU Scheduler Simulator

This project is a C-based CPU Scheduler Simulator for comparing common CPU
scheduling algorithms. It reads process workloads from text files, runs one or
more scheduling algorithms, and prints a Gantt chart, per-process statistics,
and summary performance metrics.

## Supported Algorithms

- FCFS: First Come First Serve
- SJF: Shortest Job First, non-preemptive
- Round Robin: preemptive, configurable time quantum
- Priority Scheduling: non-preemptive, lower priority number means higher priority

## Workload Format

Workload files are stored in the `inputs/` folder. Each non-comment line uses
this format:

```text
PID arrival_time burst_time priority
```

Example:

```text
P1 0 5 2
P2 1 3 1
P3 2 8 3
```

Lines starting with `#` are treated as comments.

## Build

Build the simulator with:

```bash
make
```

Clean generated files with:

```bash
make clean
```

## Run

Run all algorithms with the default workload file, `inputs/workload1.txt`:

```bash
./scheduler
```

Run a specific algorithm:

```bash
./scheduler --algo=FCFS --input=inputs/workload1.txt
./scheduler --algo=SJF --input=inputs/workload1.txt
./scheduler --algo=RR --quantum=4 --input=inputs/workload1.txt
./scheduler --algo=PRIORITY --input=inputs/workload1.txt
```

Show CLI help:

```bash
./scheduler --help
```

For compatibility with the earlier version, a positional input file is also
accepted:

```bash
./scheduler inputs/workload2.txt
```

## Output

For each scheduler run, the program prints:

- Scheduler name
- Total simulation time
- ASCII Gantt chart
- Process table with PID, arrival time, burst time, start time, completion time,
  waiting time, turnaround time, and response time
- Summary metrics:
  - average waiting time
  - average turnaround time
  - average response time
  - CPU utilization percentage
  - throughput

## Project Structure

```text
src/main.c        Program entry point and scheduler integration
src/cli.c         Command-line argument parsing
src/cli.h         CLI option structure and declarations
src/process.h     Process data structure
src/parser.c      Workload file parser
src/parser.h      Parser declarations
src/simulator.c   FCFS, SJF, Round Robin, and Priority scheduling logic
src/simulator.h   Simulator types and declarations
src/metrics.c     Performance metric calculations
src/metrics.h     Metrics structure and declarations
src/output.c      Terminal output formatting
src/output.h      Output function declarations
inputs/           Sample and test workload files
run_tests.sh      Bash test script
Makefile          Build script
```

## Testing

Run the provided test script:

```bash
./run_tests.sh
```

The tests build the project and check Round Robin behavior, Priority Scheduling
behavior, idle CPU handling, and known metric values.
