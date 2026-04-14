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

/* ── Helper: find the queue position with the shortest remaining burst ── */
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

/* ── Helper: find the queue position with the highest priority (lowest number) ── */
static int pick_priority_ready_index(int ready_queue[], int front, int back,
                                     Process processes[]) {
    int best_pos = front;
    int best_priority = processes[ready_queue[front]].priority;

    for (int pos = front + 1; pos < back; pos++) {
        int idx = ready_queue[pos];
        if (processes[idx].priority < best_priority) {
            best_priority = processes[idx].priority;
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
                /* FCFS: take the process at the front of the queue */
                running_idx = ready_queue[ready_front++];
            } else if (scheduler == SCHEDULER_SJF) {
                /* SJF: pick the process with the shortest burst */
                int best_pos = pick_sjf_ready_index(ready_queue, ready_front, ready_back,
                                                     result->processes);
                running_idx = ready_queue[best_pos];
                for (int shift = best_pos; shift < ready_back - 1; shift++) {
                    ready_queue[shift] = ready_queue[shift + 1];
                }
                ready_back--;
            } else {
                /* PRIORITY (non-preemptive): pick the highest-priority process */
                int best_pos = pick_priority_ready_index(ready_queue, ready_front, ready_back,
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
    switch (scheduler) {
        case SCHEDULER_FCFS:     return "FCFS";
        case SCHEDULER_SJF:      return "SJF";
        case SCHEDULER_RR:       return "Round Robin";
        case SCHEDULER_PRIORITY: return "Priority";
        default:                 return "Unknown";
    }
}

/* ────────────────────────────────────────────────────────────────────────────
 * Round Robin Simulation
 *
 * Uses a circular array as the ready queue.  The queue holds at most
 * MAX_PROCESSES entries at any one time (each process is enqueued at most
 * once per cycle).  One time unit is processed per loop iteration, keeping
 * the Gantt chart granularity consistent with the other schedulers.
 * ──────────────────────────────────────────────────────────────────────────*/
void run_rr_simulation(const Process input_processes[], int process_count,
                       int time_quantum, SimulationResult *result) {
    if (process_count <= 0 || result == NULL || time_quantum <= 0) {
        return;
    }

    initialize_processes(input_processes, process_count, result->processes);
    result->process_count = process_count;
    result->gantt_size    = 0;
    result->total_time    = 0;

    /* Circular ready queue — capacity MAX_PROCESSES is sufficient because
       each process can only be in the queue once at a time. */
    int  queue[MAX_PROCESSES];
    int  q_front = 0, q_back = 0, q_size = 0;

    bool added[MAX_PROCESSES];          /* has the process ever been enqueued? */
    for (int i = 0; i < process_count; i++) added[i] = false;

    int running_idx      = -1;          /* index of the currently running process */
    int quantum_remaining = 0;          /* time units left in the current quantum  */
    int completed        = 0;
    int current_time     = 0;

    /* Enqueue all processes that arrive at time 0 */
    for (int i = 0; i < process_count; i++) {
        if (result->processes[i].arrival_time == 0) {
            queue[q_back] = i;
            q_back = (q_back + 1) % MAX_PROCESSES;
            q_size++;
            added[i] = true;
        }
    }

    while (completed < process_count && current_time < MAX_SIMULATION_TIME) {

        /* 1. Enqueue processes that arrive at the current time */
        for (int i = 0; i < process_count; i++) {
            if (!added[i] && result->processes[i].arrival_time == current_time) {
                queue[q_back] = i;
                q_back = (q_back + 1) % MAX_PROCESSES;
                q_size++;
                added[i] = true;
            }
        }

        /* 2. If CPU is free, dequeue the next process */
        if (running_idx == -1 && q_size > 0) {
            running_idx = queue[q_front];
            q_front = (q_front + 1) % MAX_PROCESSES;
            q_size--;
            quantum_remaining = time_quantum;

            /* Record first start time and response time */
            if (result->processes[running_idx].start_time == -1) {
                result->processes[running_idx].start_time = current_time;
                result->processes[running_idx].response_time =
                    current_time - result->processes[running_idx].arrival_time;
            }
        }

        /* 3. Execute one time unit */
        if (running_idx != -1) {
            /* Append this time slot to the Gantt chart */
            strncpy(result->gantt_chart[result->gantt_size],
                    result->processes[running_idx].pid, MAX_PID_LEN - 1);
            result->gantt_chart[result->gantt_size][MAX_PID_LEN - 1] = '\0';
            result->gantt_size++;

            result->processes[running_idx].remaining_time--;
            quantum_remaining--;

            if (result->processes[running_idx].remaining_time == 0) {
                /* Process finished */
                result->processes[running_idx].completion_time = current_time + 1;
                result->processes[running_idx].turnaround_time =
                    result->processes[running_idx].completion_time -
                    result->processes[running_idx].arrival_time;
                /* Waiting time = turnaround - actual CPU time used */
                result->processes[running_idx].waiting_time =
                    result->processes[running_idx].turnaround_time -
                    result->processes[running_idx].burst_time;
                completed++;
                running_idx = -1;

            } else if (quantum_remaining == 0) {
                /* Quantum expired but process is not done — put it back */
                queue[q_back] = running_idx;
                q_back = (q_back + 1) % MAX_PROCESSES;
                q_size++;
                running_idx = -1;
            }

        } else {
            /* CPU is idle — no process available yet */
            strncpy(result->gantt_chart[result->gantt_size], "IDLE", MAX_PID_LEN - 1);
            result->gantt_chart[result->gantt_size][MAX_PID_LEN - 1] = '\0';
            result->gantt_size++;
        }

        current_time++;
    }

    result->total_time = current_time;
}
