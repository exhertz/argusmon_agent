#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "cpu.h"
#include "utils.h"

int cpu_model(char *model) {
  FILE *fp = fopen("/proc/cpuinfo", "r");
  if (!fp) {
    perror("file /proc/cpuinfo not opened");
    return -1;
  }

  int res = fsscanf(fp, "model name : %[^\n]", model);
  fclose(fp);

  return res;
}

double cpu_usage() {
  static unsigned long prev_total = 0;
  static unsigned long prev_idle = 0;

  FILE *fp = fopen("/proc/stat", "r");
  if (!fp) {
    perror("file /proc/stat not opened");
    return -1;
  }

  uint64_t user, nice, system, idlet, iowait, irq, softirq, steal, guest, guestnice;
  int read_bytes = fscanf(
    fp,
    "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
    &user, &nice, &system, &idlet, &iowait, &irq, &softirq, &steal, &guest, &guestnice
  );

  if (read_bytes != 10) {
    perror("not parsed params (/proc/stat)");
    fclose(fp);
    return -1;
  }

  fclose(fp);

  uint64_t total = user + nice + system + idlet + iowait + irq + softirq + steal;
  uint64_t idle = idlet + iowait;

  uint64_t total_diff = total - prev_total;
  uint64_t idle_diff = idle - prev_idle;

  // printf("%lu %lu\n", total_diff, idle_diff);
  double percentage = ((total_diff - idle_diff)*100.0) / total_diff;
  // double percentage = (total_diff - idle_diff) / total_diff;

  prev_total = total;
  prev_idle = idle;

  return percentage;
}
