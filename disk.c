#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

#include "disk.h"

typedef struct {
  uint64_t timestamp;
  uint64_t used;
} RamInfo;

static struct statvfs buf;

int get_statvfs() {
  if (statvfs("/", &buf) != 0) {
    perror("statvfs: ");
    return 0;
  }

  return 1;
}

uint64_t get_disk_total_bytes() {
  if (!get_statvfs())
    return 0;

  return buf.f_blocks * buf.f_frsize;
}

uint64_t get_disk_free_bytes() {
  if (!get_statvfs())
    return 0;

  return buf.f_bfree * buf.f_frsize;
}

uint64_t get_disk_used_bytes()  {
  return get_disk_total_bytes()-get_disk_free_bytes();
}

double get_disk_usage() {
  return (double)get_disk_used_bytes()/get_disk_total_bytes();
}
