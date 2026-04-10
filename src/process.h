#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PID_LEN 20

typedef struct {
    char pid[MAX_PID_LEN];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;

    int start_time;
    int completion_time;
    int waiting_time;
    int turnaround_time;
    int response_time;
} Process;

#endif