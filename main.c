#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "cpu.h"
#include "ram.h"
#include "disk.h"

/*
 * CPU:
 * - usage in %
 * RAM:
 * - total in kb
 * - usage in kb
 * - available in kb
 * - cached in kb
 * - free in kb
 */

#define timestamp() \
  (uint64_t)time(NULL)

/*
 * need get to run main
 */
uint64_t get_boot_timestamp() {
  FILE* stats_file;
  uint64_t boot_timestamp = 0;

  stats_file = fopen("/proc/stat", "r");

  int res = fsscanf(stats_file, "btime %llu", &boot_timestamp);
  if (!res) perror("get boot timestamp not parsed: ");
  fclose(stats_file);

  return boot_timestamp;
}

int main()
{
  char cpu_model[64];
  get_cpu_model(cpu_model);
  printf("%lu\n", get_boot_timestamp());
  printf("%s\n", cpu_model);
  while(1) {
    uint64_t total, usage, available, cached, free;
    get_ram_stats(&total, &usage, &available, &cached, &free);

    printf("%lu\n", timestamp());
    printf("CPU USAGE:\t %.2f%%\n", get_cpu_usage());
    printf("RAM USAGE:\t %lu / %lu\n", usage, total);
    printf("RAM AVAIL, CACHED, FREE:\t %lu,\t %lu,\t %lu\n", available, cached, free);
    printf("DISK USED / ALL:\t %lu / %lu\n", get_disk_used_bytes(), get_disk_total_bytes());
    printf("DISK USAGE:\t %.2f%%\n\n", get_disk_usage());
    sleep(1);
  }

  return 0;
}
