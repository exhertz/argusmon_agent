#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "types.h"
#include "utils.h"
#include "cpu.h"
#include "ram.h"

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
  (unsigned long)time(NULL)

/*
 * need get to run main
 */
unsigned long get_boot_timestamp() {
  FILE* stats_file;
  unsigned long boot_timestamp = 0;

  stats_file = fopen("/proc/stat", "r");

  int res = fsscanf(stats_file, "btime %lu", &boot_timestamp);
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
   // double t = get_cpu_usage();
   // unsigned long mem_total = get_mem_total();
    //unsigned long mem_free = get_mem_available();
    
    u64_t total, usage, available, cached, free;
    get_ram_stats(&total, &usage, &available, &cached, &free);

    printf("%lu\n", timestamp());
    printf("CPU USAGE:\t %.2f%%\n", get_cpu_usage());
    printf("RAM USAGE:\t %lu / %lu\n", usage, total);
    printf("RAM AVAIL, CACHED, FREE:\t %lu,\t %lu,\t %lu\n\n", available, cached, free);
    sleep(1);
  }

  return 0;
}
