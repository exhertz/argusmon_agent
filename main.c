#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "cpu.h"
#include "ram.h"
#include "disk.h"

#define timestamp() \
  (uint64_t)time(NULL)

/*
 * need get to run main
 */
uint64_t boot_timestamp() {
  FILE* stats_file;
  uint64_t btimestamp = 0;

  stats_file = fopen("/proc/stat", "r");

  int res = fsscanf(stats_file, "btime %llu", &btimestamp);
  if (!res) perror("get boot timestamp not parsed");
  fclose(stats_file);

  return btimestamp;
}

int main()
{
  char cpu[64];
  cpu_model(cpu);
  printf("%lu\n", boot_timestamp());
  printf("%s\n", cpu);
  while(1) {
    uint64_t total, usage, available, cached, free;
    ram_stat(&total, &usage, &available, &cached, &free);

    printf("%lu\n", timestamp());
    printf("CPU USAGE:\t %.2f%%\n", cpu_usage());
    printf("RAM USAGE:\t %lu / %lu\n", usage, total);
    printf("RAM AVAIL, CACHED, FREE:\t %lu,\t %lu,\t %lu\n", available, cached, free);
    printf("DISK USED / ALL:\t %lu / %lu\n", disk_used(), disk_total());
    printf("DISK USAGE:\t %.2f%%\n\n", disk_usage());
    sleep(1);
  }

  return 0;
}
