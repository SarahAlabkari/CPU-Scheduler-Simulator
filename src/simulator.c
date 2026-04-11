#include "simulator.h"
#include <stdbool.h>
#include <string.h>

static void initialize_processes(const Process input_processes[], int process_count,
                                 Process processes[]) {
    for (int i = 0; i < process_count; i++) {
        processes[i] = input_processes[i];
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].start_time = -1;
        processes[i].completion_time = -1;
        processes[i].waiting_time = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time = -1;
    }
}

static int pick_sjf_ready_index(int ready_queue[], int front, int back, Process processes[]) {
    int best_pos = front;
    int best_idx = ready_queue[front];
    int best_burst = processes[best_idx].burst_time;

    for (int pos = front + 1; pos < back; pos++) {
        int idx = ready_queue[pos];
        int burst = processes[idx].burst_time;
        if (burst < best_burst) {
            best_burst = burst;
            best_idx = idx;
            best_pos = pos;
        }
    }

    return best_pos;
}

void run_simulation(const Process input_processes[], int process_count,
                    SchedulerType scheduler, SimulationResult *result) {
    if (process_count <= 0 || result == NULL) {
        return;
    }

    initialize_processes(input_processes, process_count, result->processes);
    result->process_count = process_count;
    result->gantt_size = 0;
    result->total_time = 0;

    int ready_queue[MAX_PROCESSES];
    int ready_front = 0;
    int ready_back = 0;
    int completed = 0;
    int current_time = 0;
    int running_idx = -1;
    bool added[MAX_PROCESSES] = {false};

    while (completed < process_count && current_time < MAX_SIMULATION_TIME) {
        for (int i = 0; i < process_count; i++) {
            if (!added[i] && result->processes[i].arrival_time == current_time) {
                ready_queue[ready_back++] = i;
                added[i] = true;
            }
        }

        if (running_idx == -1 && ready_front < ready_back) {
            if (scheduler == SCHEDULER_FCFS) {
                running_idx = ready_queue[ready_front++];
            } else {
                int best_pos = pick_sjf_ready_index(ready_queue, ready_front, ready_back,
                                                     result->processes);
                running_idx = ready_queue[best_pos];

                for (int shift = best_pos; shift < ready_back - 1; shift++) {
                    ready_queue[shift] = ready_queue[shift + 1];
                }
                ready_back--;
            }

            if (result->processes[running_idx].start_time == -1) {
                result->processes[running_idx].start_time = current_time;
                result->processes[running_idx].response_time = current_time -
                    result->processes[running_idx].arrival_time;
            }
        }

        if (running_idx != -1) {
            strncpy(result->gantt_chart[result->gantt_size],
                    result->processes[running_idx].pid,
                    MAX_PID_LEN - 1);
            result->gantt_chart[result->gantt_size][MAX_PID_LEN - 1] = '\0';
            result->gantt_size++;

            result->processes[running_idx].remaining_time--;

            if (result->processes[running_idx].remaining_time <= 0) {
                result->processes[running_idx].completion_time = current_time + 1;
                result->processes[running_idx].turnaround_time =
                    result->processes[running_idx].completion_time -
                    result->processes[running_idx].arrival_time;
                result->processes[running_idx].waiting_time =
                    result->processes[running_idx].start_time -
                    result->processes[running_idx].arrival_time;
                completed++;
                running_idx = -1;
            }
        } else {
            strncpy(result->gantt_chart[result->gantt_size], "IDLE", MAX_PID_LEN - 1);
            result->gantt_chart[result->gantt_size][MAX_PID_LEN - 1] = '\0';
            result->gantt_size++;
        }

        current_time++;
    }

    result->total_time = current_time;
}

const char *get_scheduler_name(SchedulerType scheduler) {
    return scheduler == SCHEDULER_SJF ? "SJF" : "FCFS";
}
