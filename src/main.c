#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "simulator.h"

static void print_usage(const char *program_name) {
    printf("Usage: %s [workload_file]\n", program_name);
    printf("If no workload file is supplied, the default is inputs/workload1.txt\n");
}

static void print_simulation_result(const SimulationResult *result) {
    printf("Total time: %d\n", result->total_time);
    printf("Execution timeline:\n");
    for (int t = 0; t < result->gantt_size; t++) {
        printf("Time %d: %s\n", t, result->gantt_chart[t]);
    }

    printf("\nProcess stats:\n");
    for (int i = 0; i < result->process_count; i++) {
        const Process *p = &result->processes[i];
        printf("PID: %s, Arrival: %d, Burst: %d, Start: %d, Completion: %d, Waiting: %d, Turnaround: %d, Response: %d\n",
               p->pid,
               p->arrival_time,
               p->burst_time,
               p->start_time,
               p->completion_time,
               p->waiting_time,
               p->turnaround_time,
               p->response_time);
    }
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

    printf("Parsed %d processes from '%s'.\n\n", count, workload_file);

    for (int i = 0; i < count; i++) {
        printf("PID: %s, Arrival: %d, Burst: %d, Priority: %d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority);
    }

    printf("\n=== Running FCFS ===\n");
    SimulationResult fcfs_result;
    run_simulation(processes, count, SCHEDULER_FCFS, &fcfs_result);
    print_simulation_result(&fcfs_result);

    printf("\n=== Running SJF ===\n");
    SimulationResult sjf_result;
    run_simulation(processes, count, SCHEDULER_SJF, &sjf_result);
    print_simulation_result(&sjf_result);

    return 0;
}