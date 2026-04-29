#include <stdio.h>
#include "output.h"

void print_gantt_chart(const SimulationResult *result) {
    printf("\nGantt Chart:\n\n");

    printf("Time:");
    for (int t = 0; t < result->gantt_size; t++) {
        printf(" %4d", t);
    }
    printf("\n");

    printf("CPU :");
    for (int t = 0; t < result->gantt_size; t++) {
        printf(" %4s", result->gantt_chart[t]);
    }
    printf("\n\n");
}

void print_process_table(const SimulationResult *result) {
    printf("%-6s %8s %7s %7s %11s %9s %12s %9s\n",
           "PID", "Arrival", "Burst", "Start", "Completion",
           "Waiting", "Turnaround", "Response");
    printf("%-6s %8s %7s %7s %11s %9s %12s %9s\n",
           "------", "-------", "-----", "-----", "----------",
           "-------", "----------", "--------");

    for (int i = 0; i < result->process_count; i++) {
        const Process *p = &result->processes[i];
        printf("%-6s %8d %7d %7d %11d %9d %12d %9d\n",
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

void print_metrics(const Metrics *metrics) {
    printf("\n--- Metrics ---\n");
    printf("Average Waiting Time     : %.2f\n", metrics->avg_waiting_time);
    printf("Average Turnaround Time  : %.2f\n", metrics->avg_turnaround_time);
    printf("Average Response Time    : %.2f\n", metrics->avg_response_time);
    printf("CPU Utilization          : %.2f%%\n", metrics->cpu_utilization);
    printf("Throughput               : %.4f processes/unit\n\n",
           metrics->throughput);
}

void print_simulation_result(const SimulationResult *result) {
    Metrics metrics;

    printf("Total simulation time: %d\n", result->total_time);
    print_gantt_chart(result);
    print_process_table(result);

    calculate_metrics(result, &metrics);
    print_metrics(&metrics);
}
