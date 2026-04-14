#include <stdio.h>
#include "parser.h"
#include "simulator.h"
#include "output.h"

#define DEFAULT_TIME_QUANTUM 2

static void print_usage(const char *program_name) {
    printf("Usage: %s [workload_file]\n", program_name);
    printf("If no workload file is supplied, the default is inputs/workload1.txt\n");
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *workload_file = "inputs/workload1.txt";
    if (argc == 2) {
        workload_file = argv[1];
    }

    Process processes[MAX_PROCESSES];
    int count = parse_workload(workload_file, processes, MAX_PROCESSES);
    if (count < 0) {
        return 1;
    }

    printf("Loaded %d processes from '%s'.\n", count, workload_file);
    printf("\n%-6s %8s %6s %9s\n", "PID", "Arrival", "Burst", "Priority");
    for (int i = 0; i < count; i++) {
        printf("%-6s %8d %6d %9d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority);
    }

    /* ── FCFS ──────────────────────────────────────────────────────────── */
    printf("\n=== FCFS ===\n");
    SimulationResult fcfs_result;
    run_simulation(processes, count, SCHEDULER_FCFS, &fcfs_result);
    print_simulation_result(&fcfs_result);

    /* ── SJF (non-preemptive) ──────────────────────────────────────────── */
    printf("\n=== SJF (Non-Preemptive) ===\n");
    SimulationResult sjf_result;
    run_simulation(processes, count, SCHEDULER_SJF, &sjf_result);
    print_simulation_result(&sjf_result);

    /* ── Round Robin ───────────────────────────────────────────────────── */
    printf("\n=== Round Robin (quantum = %d) ===\n", DEFAULT_TIME_QUANTUM);
    SimulationResult rr_result;
    run_rr_simulation(processes, count, DEFAULT_TIME_QUANTUM, &rr_result);
    print_simulation_result(&rr_result);

    /* ── Priority (non-preemptive) ─────────────────────────────────────── */
    printf("\n=== Priority Scheduling (Non-Preemptive) ===\n");
    SimulationResult pri_result;
    run_simulation(processes, count, SCHEDULER_PRIORITY, &pri_result);
    print_simulation_result(&pri_result);

    return 0;
}