#pragma once 
#include <stdint.h>

#if 0
uint64_t get_mem_available_bytes();
uint64_t get_mem_free_bytes();
#endif

int ram_stat(uint64_t *total, uint64_t *usage, uint64_t *available, uint64_t *cached, uint64_t *free);
