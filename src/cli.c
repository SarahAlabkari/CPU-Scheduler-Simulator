#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

static int parse_positive_int(const char *text, int *value) {
    char *end = NULL;
    long parsed = strtol(text, &end, 10);

    if (text[0] == '\0' || end == text || *end != '\0' || parsed <= 0) {
        return -1;
    }

    *value = (int)parsed;
    return 0;
}

static int parse_scheduler_name(const char *name, SchedulerType *scheduler) {
    if (strcmp(name, "FCFS") == 0) {
        *scheduler = SCHEDULER_FCFS;
    } else if (strcmp(name, "SJF") == 0) {
        *scheduler = SCHEDULER_SJF;
    } else if (strcmp(name, "RR") == 0) {
        *scheduler = SCHEDULER_RR;
    } else if (strcmp(name, "PRIORITY") == 0) {
        *scheduler = SCHEDULER_PRIORITY;
    } else {
        return -1;
    }

    return 0;
}

static void set_defaults(CLIOptions *options) {
    strncpy(options->input_file, DEFAULT_INPUT_FILE, sizeof(options->input_file) - 1);
    options->input_file[sizeof(options->input_file) - 1] = '\0';
    options->run_all = 1;
    options->scheduler = SCHEDULER_FCFS;
    options->quantum = DEFAULT_RR_QUANTUM;
    options->show_help = 0;
}

int parse_cli_args(int argc, char *argv[], CLIOptions *options) {
    int positional_input_used = 0;

    if (options == NULL) {
        return -1;
    }

    set_defaults(options);

    for (int i = 1; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "--help") == 0) {
            options->show_help = 1;
            return 0;
        } else if (strncmp(arg, "--algo=", 7) == 0) {
            const char *name = arg + 7;
            if (parse_scheduler_name(name, &options->scheduler) != 0) {
                fprintf(stderr, "Error: invalid algorithm '%s'.\n", name);
                return -1;
            }
            options->run_all = 0;
        } else if (strncmp(arg, "--input=", 8) == 0) {
            const char *path = arg + 8;
            if (path[0] == '\0') {
                fprintf(stderr, "Error: --input requires a file path.\n");
                return -1;
            }
            strncpy(options->input_file, path, sizeof(options->input_file) - 1);
            options->input_file[sizeof(options->input_file) - 1] = '\0';
        } else if (strncmp(arg, "--quantum=", 10) == 0) {
            if (parse_positive_int(arg + 10, &options->quantum) != 0) {
                fprintf(stderr, "Error: --quantum must be a positive integer.\n");
                return -1;
            }
        } else if (strncmp(arg, "--", 2) == 0) {
            fprintf(stderr, "Error: unknown option '%s'.\n", arg);
            return -1;
        } else if (!positional_input_used) {
            strncpy(options->input_file, arg, sizeof(options->input_file) - 1);
            options->input_file[sizeof(options->input_file) - 1] = '\0';
            positional_input_used = 1;
        } else {
            fprintf(stderr, "Error: unexpected argument '%s'.\n", arg);
            return -1;
        }
    }

    return 0;
}

void print_usage(const char *program_name) {
    printf("Usage:\n");
    printf("  %s\n", program_name);
    printf("  %s --algo=FCFS --input=inputs/workload1.txt\n", program_name);
    printf("  %s --algo=SJF --input=inputs/workload1.txt\n", program_name);
    printf("  %s --algo=RR --quantum=4 --input=inputs/workload1.txt\n", program_name);
    printf("  %s --algo=PRIORITY --input=inputs/workload1.txt\n", program_name);
    printf("  %s --help\n", program_name);
    printf("\nOptions:\n");
    printf("  --algo=FCFS|SJF|RR|PRIORITY  Run one scheduling algorithm only.\n");
    printf("  --input=FILE                 Workload file to load.\n");
    printf("  --quantum=N                  Round Robin time quantum. Default: %d.\n",
           DEFAULT_RR_QUANTUM);
    printf("  --help                       Show this help message.\n");
    printf("\nDefaults:\n");
    printf("  input file: %s\n", DEFAULT_INPUT_FILE);
    printf("  mode: run all algorithms\n");
    printf("  Round Robin quantum: %d\n", DEFAULT_RR_QUANTUM);
}
