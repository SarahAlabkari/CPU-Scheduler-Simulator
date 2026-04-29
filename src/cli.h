#ifndef CLI_H
#define CLI_H

#include "simulator.h"

#define DEFAULT_INPUT_FILE "inputs/workload1.txt"
#define DEFAULT_RR_QUANTUM 2

typedef struct {
    char input_file[256];
    int run_all;
    SchedulerType scheduler;
    int quantum;
    int show_help;
} CLIOptions;

int parse_cli_args(int argc, char *argv[], CLIOptions *options);
void print_usage(const char *program_name);

#endif /* CLI_H */
