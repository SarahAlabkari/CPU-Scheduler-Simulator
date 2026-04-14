#include <stdio.h>
#include "output.h"

/* ── Gantt chart ─────────────────────────────────────────────────────────── */
void print_gantt_chart(const SimulationResult *result) {
    printf("Gantt Chart:\n");
    for (int t = 0; t < result->gantt_size; t++) {
        printf("  Time %3d: %s\n", t, result->gantt_chart[t]);
    }
}

/* ── Per-process table ───────────────────────────────────────────────────── */
void print_process_table(const SimulationResult *result) {
    printf("\n%-6s %8s %6s %6s %11s %8s %11s %9s\n",
           "PID", "Arrival", "Burst", "Start", "Completion",
           "Waiting", "Turnaround", "Response");
    printf("%-6s %8s %6s %6s %11s %8s %11s %9s\n",
           "------", "-------", "-----", "-----", "----------",
           "-------", "----------", "--------");

    for (int i = 0; i < result->process_count; i++) {
        const Process *p = &result->processes[i];
        printf("%-6s %8d %6d %6d %11d %8d %11d %9d\n",
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

/* ── Aggregate metrics ───────────────────────────────────────────────────── */
void print_metrics(const Metrics *metrics) {
    printf("\nMetrics:\n");
    printf("  Average Waiting Time    : %.2f\n", metrics->avg_waiting_time);
    printf("  Average Turnaround Time : %.2f\n", metrics->avg_turnaround_time);
    printf("  Average Response Time   : %.2f\n", metrics->avg_response_time);
    printf("  CPU Utilization         : %.2f%%\n", metrics->cpu_utilization);
    printf("  Throughput              : %.4f processes/unit\n", metrics->throughput);
}

/* ── Convenience wrapper ─────────────────────────────────────────────────── */
void print_simulation_result(const SimulationResult *result) {
    printf("Total simulation time: %d\n", result->total_time);
    print_gantt_chart(result);
    print_process_table(result);

    Metrics m;
    calculate_metrics(result, &m);
    print_metrics(&m);
}
