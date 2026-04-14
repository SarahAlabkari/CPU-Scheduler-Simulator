#include <stdlib.h>
#include "metrics.h"

/*
 * calculate_metrics
 *
 * Walks the completed SimulationResult and computes all five aggregate
 * metrics.  The per-process waiting_time, turnaround_time, and response_time
 * fields must already be filled in by the scheduler (run_simulation /
 * run_rr_simulation).
 */
void calculate_metrics(const SimulationResult *result, Metrics *metrics) {
    if (result == NULL || metrics == NULL) {
        return;
    }

    double total_waiting    = 0.0;
    double total_turnaround = 0.0;
    double total_response   = 0.0;
    int    busy_time        = 0;   /* total CPU-busy ticks = sum of burst times */
    int    n                = result->process_count;

    for (int i = 0; i < n; i++) {
        total_waiting    += result->processes[i].waiting_time;
        total_turnaround += result->processes[i].turnaround_time;
        total_response   += result->processes[i].response_time;
        busy_time        += result->processes[i].burst_time;
    }

    /* Averages */
    metrics->avg_waiting_time    = (n > 0) ? total_waiting    / n : 0.0;
    metrics->avg_turnaround_time = (n > 0) ? total_turnaround / n : 0.0;
    metrics->avg_response_time   = (n > 0) ? total_response   / n : 0.0;

    /* CPU Utilization: fraction of total time the CPU was running a process */
    metrics->cpu_utilization = (result->total_time > 0)
        ? (busy_time * 100.0) / result->total_time
        : 0.0;

    /* Throughput: completed processes per unit time */
    metrics->throughput = (result->total_time > 0)
        ? (double)n / result->total_time
        : 0.0;
}
