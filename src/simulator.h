#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "parser.h"

#define MAX_SIMULATION_TIME 10000

typedef enum {
    SCHEDULER_FCFS,
    SCHEDULER_SJF
} SchedulerType;

typedef struct {
    Process processes[MAX_PROCESSES];
    int process_count;
    int gantt_size;
    int total_time;
    char gantt_chart[MAX_SIMULATION_TIME][MAX_PID_LEN];
} SimulationResult;

void run_simulation(const Process input_processes[], int process_count,
                    SchedulerType scheduler, SimulationResult *result);

const char *get_scheduler_name(SchedulerType scheduler);

#endif // SIMULATOR_H
