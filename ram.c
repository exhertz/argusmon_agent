#include <stdio.h>
#include <stdlib.h>

#include "ram.h"
#include "utils.h"

/* this logic release on free and btop */
/* MemTotal - get_mem_available()  <- to get usage*/
#if 0
uint64_t get_mem_available() {
  FILE *stats_file = fopen("/proc/meminfo", "r");
  uint64_t mem_available = 0;

  int res = fsscanf(stats_file, "MemAvailable: %lu kB", &mem_available);
  if (!res) perror("get mem avail. not parsed: ");
  fclose(stats_file);
  return mem_available * 1024; // return in bytes
}
#endif

/* this logic release on htop */
/* MemTotal - get_mem_free()  <- to get usage */
#if 0
uint64_t get_mem_free() {
  FILE* stats_file = fopen("/proc/meminfo", "r");
  uint64_t mem_free = 0;
  uint64_t mem_buffers = 0;
  uint64_t mem_cached = 0;
  uint64_t mem_srecl = 0;

  int res = 0;
  res += fsscanf(stats_file, "MemFree: %lu kB", &mem_free);
  res += fsscanf(stats_file, "Buffers: %lu kB", &mem_buffers);
  res += fsscanf(stats_file, "Cached: %lu kB", &mem_cached);
  res += fsscanf(stats_file, "SReclaimable: %lu kB", &mem_srecl);
  fclose(stats_file);

  return (res == 4 ? ((mem_free + mem_buffers + mem_cached + mem_srecl) * 1024) : 0); // return in bytes
}
#endif

int ram_stat(
  uint64_t *total, uint64_t *usage, uint64_t *available, uint64_t *cached, uint64_t *free
) {
  FILE *stats = fopen("/proc/meminfo", "r");
  int res = 0;

  res += fsscanf(stats, "MemTotal: %lu kB", total);
  res += fsscanf(stats, "MemFree: %lu kB", free);
  res += fsscanf(stats, "MemAvailable: %lu kB", available);
  res += fsscanf(stats, "Cached: %lu kB", cached);
  
  fclose(stats);

  if (res != 4) {
    return 1;
  }

  *total = *total * 1024;
  *free = *free * 1024;
  *available = *available * 1024;
  *cached = *cached * 1024;

  *usage = *total - *available;

  return 0;
}
