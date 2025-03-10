#include <stdio.h>
#include <stdlib.h>

#include "ram.h"
#include "utils.h"

unsigned long get_mem_total() {
  FILE* stats_file = fopen("/proc/meminfo", "r");
  unsigned long mem_total = 0;

  int res = fsscanf(stats_file, "MemTotal: %lu kB", &mem_total);
  fclose(stats_file);

  return mem_total;
}

/* this logic release on free and btop */
/* MemTotal - get_mem_available()  <- to get usage*/
unsigned long get_mem_available() {
  FILE *stats_file = fopen("/proc/meminfo", "r");
  unsigned long mem_available = 0;

  int res = fsscanf(stats_file, "MemAvailable: %lu kB", &mem_available);
  fclose(stats_file);
  return mem_available;
}

/* this logic release on htop */
/* MemTotal - get_mem_free()  <- to get usage */
unsigned long get_mem_free() {
  FILE* stats_file = fopen("/proc/meminfo", "r");
  unsigned long mem_free = 0;
  unsigned long mem_buffers = 0;
  unsigned long mem_cached = 0;
  unsigned long mem_srecl = 0;

  int res = 0;
  res += fsscanf(stats_file, "MemFree: %lu kB", &mem_free);
  res += fsscanf(stats_file, "Buffers: %lu kB", &mem_buffers);
  res += fsscanf(stats_file, "Cached: %lu kB", &mem_cached);
  res += fsscanf(stats_file, "SReclaimable: %lu kB", &mem_srecl);
  fclose(stats_file);

  return (res == 4 ? (mem_free + mem_buffers + mem_cached + mem_srecl) : 0);
}
