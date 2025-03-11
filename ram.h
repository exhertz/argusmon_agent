#pragma once 
#include "types.h"

#if 0
unsigned long get_mem_available();
unsigned long get_mem_free();
#endif

int get_ram_stats(u64_t *total, u64_t *usage, u64_t *available, u64_t *cached, u64_t *free);
