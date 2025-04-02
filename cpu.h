#pragma once

double cpu_usage();
int cpu_model(char *model);
int cpu_stat(uint64_t *total, uint64_t *idle);
