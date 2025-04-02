#include <stdio.h>
#include <stdlib.h>
#include <sys/statvfs.h>

#include "disk.h"

static struct statvfs buf;

int get_statvfs() {
  if (statvfs("/", &buf) != 0) {
    perror("statvfs");
    return -1;
  }

  return 0;
}

uint64_t disk_total() {
  if (get_statvfs() != 0)
    return 0;

  return buf.f_blocks * buf.f_frsize;
}

uint64_t disk_free() {
  if (get_statvfs() != 0)
    return 0;

  return buf.f_bfree * buf.f_frsize;
}

uint64_t disk_used()  {
  return disk_total() - disk_free();
}

double disk_usage() {
  return (double)disk_used()/disk_total();
}
