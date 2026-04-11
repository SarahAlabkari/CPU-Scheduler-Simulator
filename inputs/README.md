# Workload Inputs

This folder contains sample workload files for the CPU scheduler simulator.

## Format

Each line in a workload file should have the following fields separated by whitespace:

```
PID arrival_time burst_time priority
```

Example:

```
P1 0 5 2
P2 1 3 1
P3 2 8 3
P4 3 6 2
```

## Notes

- `PID` is a process identifier string.
- `arrival_time` is the time when the process enters the ready queue.
- `burst_time` is the CPU execution time required.
- `priority` is included for future scheduler support.

## Usage

Run the simulator with the default file:

```bash
./scheduler
```

Or specify a different workload file:

```bash
./scheduler inputs/workload2.txt
```
