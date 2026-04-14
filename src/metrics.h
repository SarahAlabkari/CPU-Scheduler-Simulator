#ifndef METRICS_H
#define METRICS_H

#include "simulator.h"

/* Aggregated performance metrics for one simulation run. */
typedef struct {
    double avg_waiting_time;     /* average waiting time across all processes   */
    double avg_turnaround_time;  /* average turnaround time                     */
    double avg_response_time;    /* average response time (first-start - arrival) */
    double cpu_utilization;      /* percentage of time the CPU was busy (0-100) */
    double throughput;           /* completed processes per unit of time        */
} Metrics;

/*
 * calculate_metrics
 *
 * Reads the completed SimulationResult and fills every field of *metrics.
 *
 * Formulae:
 *   Waiting Time     = already stored per-process in result->processes[i].waiting_time
 *   Turnaround Time  = completion_time - arrival_time  (stored per process)
 *   Response Time    = start_time - arrival_time       (stored per process)
 *   CPU busy time    = sum of all burst_times (every process runs to completion once)
 *   CPU Utilization  = busy_time / total_time * 100
 *   Throughput       = process_count / total_time
 */
void calculate_metrics(const SimulationResult *result, Metrics *metrics);

#endif /* METRICS_H */
