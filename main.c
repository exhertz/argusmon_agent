#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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

  printf("%lu\n", get_boot_timestamp());
  printf("%lu\n", get_mem_total());
  while(1) {
    double t = get_cpu_usage();
    unsigned long mem_total = get_mem_total();
    unsigned long mem_free = get_mem_available();
    printf("CPU USAGE: %lu\t%.2f%%\n", timestamp(), t);
    printf("RAM USAGE: %lu / %lu\n\n", mem_total-mem_free, mem_total);
    sleep(1);
  }

  return 0;
}
