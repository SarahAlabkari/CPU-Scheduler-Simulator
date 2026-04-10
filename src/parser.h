#ifndef PARSER_H
#define PARSER_H

#include "process.h"

#define MAX_PROCESSES 100

int parse_workload(const char *filename, Process processes[], int max_processes);

#endif