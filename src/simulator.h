#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "parser.h"

#define MAX_SIMULATION_TIME 10000

typedef enum {
    SCHEDULER_FCFS,
    SCHEDULER_SJF,
    SCHEDULER_RR,
    SCHEDULER_PRIORITY
} SchedulerType;

typedef struct {
    Process processes[MAX_PROCESSES];
    int process_count;
    int gantt_size;
    int total_time;
    char gantt_chart[MAX_SIMULATION_TIME][MAX_PID_LEN];
} SimulationResult;

/* Runs FCFS or SJF (non-preemptive) or Priority (non-preemptive) simulation. */
void run_simulation(const Process input_processes[], int process_count,
                    SchedulerType scheduler, SimulationResult *result);

/* Runs Round Robin simulation with the given time quantum. */
void run_rr_simulation(const Process input_processes[], int process_count,
                       int time_quantum, SimulationResult *result);

const char *get_scheduler_name(SchedulerType scheduler);

#endif // SIMULATOR_H
