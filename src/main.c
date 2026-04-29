#include <stdio.h>
#include "cli.h"
#include "parser.h"
#include "simulator.h"
#include "output.h"

static void print_loaded_processes(const Process processes[], int count,
                                   const char *workload_file) {
    printf("Loaded %d processes from '%s'.\n", count, workload_file);
    printf("\n%-6s %8s %6s %9s\n", "PID", "Arrival", "Burst", "Priority");

    for (int i = 0; i < count; i++) {
        printf("%-6s %8d %6d %9d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority);
    }
}

static const char *get_cli_scheduler_label(SchedulerType scheduler) {
    switch (scheduler) {
        case SCHEDULER_FCFS:     return "FCFS";
        case SCHEDULER_SJF:      return "SJF";
        case SCHEDULER_RR:       return "RR";
        case SCHEDULER_PRIORITY: return "PRIORITY";
        default:                 return "UNKNOWN";
    }
}

static void print_scheduler_header(SchedulerType scheduler, int quantum) {
    printf("\n==============================\n");

    if (scheduler == SCHEDULER_RR) {
        printf("=== Round Robin Scheduling (quantum = %d) ===\n", quantum);
    } else {
        printf("=== %s Scheduling ===\n", get_scheduler_name(scheduler));
    }

    printf("==============================\n");
}

static void run_and_print_scheduler(const Process processes[], int count,
                                    SchedulerType scheduler, int quantum) {
    SimulationResult result;

    print_scheduler_header(scheduler, quantum);

    if (scheduler == SCHEDULER_RR) {
        run_rr_simulation(processes, count, quantum, &result);
    } else {
        run_simulation(processes, count, scheduler, &result);
    }

    print_simulation_result(&result);
}

int main(int argc, char *argv[]) {
    CLIOptions options;

    if (parse_cli_args(argc, argv, &options) != 0) {
        print_usage(argv[0]);
        return 1;
    }

    if (options.show_help) {
        print_usage(argv[0]);
        return 0;
    }

    Process processes[MAX_PROCESSES];
    int count = parse_workload(options.input_file, processes, MAX_PROCESSES);
    if (count < 0) {
        return 1;
    }

    print_loaded_processes(processes, count, options.input_file);

    if (options.run_all) {
        printf("\nRunning All Scheduling Algorithms\n");
        run_and_print_scheduler(processes, count, SCHEDULER_FCFS, options.quantum);
        run_and_print_scheduler(processes, count, SCHEDULER_SJF, options.quantum);
        run_and_print_scheduler(processes, count, SCHEDULER_RR, options.quantum);
        run_and_print_scheduler(processes, count, SCHEDULER_PRIORITY, options.quantum);
    } else {
        printf("\nRunning Scheduler: %s\n",
               get_cli_scheduler_label(options.scheduler));
        run_and_print_scheduler(processes, count, options.scheduler, options.quantum);
    }

    return 0;
}
