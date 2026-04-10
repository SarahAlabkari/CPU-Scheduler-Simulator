#include <stdio.h>
#include "parser.h"

int main() {
    Process processes[MAX_PROCESSES];

    int count = parse_workload("inputs/workload1.txt", processes, MAX_PROCESSES);

    if (count < 0) {
        return 1;
    }

    printf("Parsed %d processes:\n", count);

    for (int i = 0; i < count; i++) {
        printf("PID: %s, Arrival: %d, Burst: %d, Priority: %d\n",
               processes[i].pid,
               processes[i].arrival_time,
               processes[i].burst_time,
               processes[i].priority);
    }

    return 0;
}