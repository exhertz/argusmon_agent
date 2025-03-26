#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "utils.h"
#include "cpu.h"
#include "ram.h"
#include "disk.h"
#include "net.h"

#define timestamp() \
  (uint64_t)time(NULL)

/*
 * need get to run main
 */
uint64_t boot_timestamp() {
  FILE *fp = fopen("/proc/stat", "r");
  if (!fp) {
    perror("file /proc/stat not opened");
    return 0;
  }

  uint64_t btimestamp = 0;
  int res = fsscanf(fp, "btime %llu", &btimestamp);
  if (!res) perror("not parsed btime (/proc/stat)");
  fclose(fp);

  return btimestamp;
}

int main()
{
  char cpu[64] = {0};
  if (cpu_model(cpu) != 0) {
    printf("cpu model not parsed\n");
  } else {
    printf("CPU MODEL: %s\n", cpu);
  }

  char iface[16] = {0};
  if (default_iface(iface) != 0) {
    printf("no return iface\n");
  } else {
    printf("NET INTERFACE: %s\n", iface);
  }

  printf("%lu\n", boot_timestamp());

  uint64_t total, usage, available, cached, free;
  uint64_t rx, tx;
  while(1) {
    int resram = ram_stat(&total, &usage, &available, &cached, &free);
    if (resram != 0) {
      printf("ram_stat not parse value!\n");
    }

    int resnet = net_stat(iface, &rx, &tx);
    if (resnet != 0) {
      printf("net_stat not parse value!\n");
    }

    printf("%lu\n", timestamp());
    printf("CPU USAGE:\t %.2f%%\n", cpu_usage());
    printf("RAM USAGE:\t %lu / %lu\n", usage, total);
    printf("RAM AVAIL, CACHED, FREE:\t %lu,\t %lu,\t %lu\n", available, cached, free);
    printf("DISK USED / ALL:\t %lu / %lu\n", disk_used(), disk_total());
    printf("DISK USAGE:\t %.2f%%\n", disk_usage());
    printf("NET DOWNLOAD, UPLOAD:\t %lu, %lu\n", rx, tx);
    printf("\n");
    sleep(1);
  }

  return 0;
}
