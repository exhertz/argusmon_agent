#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"

double get_cpu_usage() {
  FILE* stats_file;
  static unsigned long prev_total = 0;
  static unsigned long prev_idle = 0;
  unsigned long user, nice, system, idle_t, iowait, irq, softirq, steal, guest, guest_nice;

  stats_file = fopen("/proc/stat", "r");
  if (stats_file == NULL) {
    perror("/proc/stat not opened: ");
    return -1;
  }

  int read_bytes = fscanf(
    stats_file,
    "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
    &user, &nice, &system, &idle_t, &iowait, &irq, &softirq, &steal, &guest, &guest_nice
  );

  if (read_bytes != 10) {
    perror("/proc/stat not parsed: ");
    fclose(stats_file);
    return -1;
  }

  fclose(stats_file);

  unsigned long total = user + nice + system + idle_t + iowait + irq + softirq + steal;
  unsigned long idle = idle_t + iowait;

  unsigned long total_diff = total - prev_total;
  unsigned long idle_diff = idle - prev_idle;

  // printf("%lu %lu\n", total_diff, idle_diff);
  double percentage = ((total_diff - idle_diff)*100.0) / total_diff;
  // double percentage = (total_diff - idle_diff) / total_diff;

  prev_total = total;
  prev_idle = idle;

  return percentage;
}
