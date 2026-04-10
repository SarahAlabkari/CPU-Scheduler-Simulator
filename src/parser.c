#include <stdio.h>
#include <string.h>
#include "parser.h"

int parse_workload(const char *filename, Process processes[], int max_processes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return -1;
    }

    int count = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == '\n' || line[0] == '#') {
            continue;
        }

        if (count >= max_processes) {
            printf("Warning: Maximum number of processes reached.\n");
            break;
        }

        Process p;

        int fields = sscanf(
            line,
            "%19s %d %d %d",
            p.pid,
            &p.arrival_time,
            &p.burst_time,
            &p.priority
        );

        if (fields != 4) {
            printf("Warning: Invalid line skipped: %s", line);
            continue;
        }

        p.remaining_time = p.burst_time;
        p.start_time = -1;
        p.completion_time = -1;
        p.waiting_time = 0;
        p.turnaround_time = 0;
        p.response_time = 0;

        processes[count] = p;
        count++;
    }

    fclose(file);
    return count;
}