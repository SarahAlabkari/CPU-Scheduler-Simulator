#ifndef OUTPUT_H
#define OUTPUT_H

#include "simulator.h"
#include "metrics.h"

/*
 * print_gantt_chart
 *   Prints the Gantt chart (one time slot per line) for a completed simulation.
 */
void print_gantt_chart(const SimulationResult *result);

/*
 * print_process_table
 *   Prints per-process statistics (arrival, burst, start, completion,
 *   waiting, turnaround, response) in a tabular format.
 */
void print_process_table(const SimulationResult *result);

/*
 * print_metrics
 *   Prints the five aggregate metrics stored in a Metrics struct.
 */
void print_metrics(const Metrics *metrics);

/*
 * print_simulation_result
 *   Convenience wrapper: calls print_gantt_chart, print_process_table, and
 *   print_metrics in order.  Metrics are calculated internally.
 */
void print_simulation_result(const SimulationResult *result);

#endif /* OUTPUT_H */
