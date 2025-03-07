#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

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

// fsscanf - file string scan formated
int fsscanf (FILE *buff, const char *format, ...) {
  va_list args;
  char line[256];

  while(fgets(line, sizeof(line), buff)) {
    va_start(args, format);
    int res = vsscanf(line, format, args);
    va_end(args);

    if (res > 0) {
      return res;
    }
  }

  return 0;
}


/*
 * need get to run main
 */
unsigned long get_boot_timestamp() {
  FILE* stats_file;
  unsigned long boot_timestamp = 0;

  stats_file = fopen("/proc/stat", "r");

  int res = fsscanf(stats_file, "btime %lu", &boot_timestamp);
  fclose(stats_file);

  return (res ? boot_timestamp : -1);
}

unsigned long get_mem_total() {
  FILE* stats_file = fopen("/proc/meminfo", "r");
  unsigned long mem_total = 0;

  int res = fsscanf(stats_file, "MemTotal: %lu kB", &mem_total);
  fclose(stats_file);

  return (res ? mem_total : -1);
}

/* this logic release on free and btop */
/* MemTotal - get_mem_available()  <- to get usage*/
unsigned long get_mem_available() {
  FILE *stats_file = fopen("/proc/meminfo", "r");
  unsigned long mem_available = 0;

  int res = fsscanf(stats_file, "MemAvailable: %lu kB", &mem_available);
  fclose(stats_file);
  return (res ? mem_available : -1);
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

  return (res == 4 ? (mem_free + mem_buffers + mem_cached + mem_srecl) : -1);
}

double get_cpu_usage() {
  FILE* stats_file;
  static unsigned long prev_total = 0;
  static unsigned long prev_idle = 0;
  unsigned long user, nice, system, idle_t, iowait, irq, softirq, steal, guest, guest_nice;

  stats_file = fopen("/proc/stat", "r");
  if (stats_file == NULL) {
    printf("/proc/stat not opened!\n");
    return -1;
  }

  int read_bytes = fscanf(
    stats_file,
    "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
    &user, &nice, &system, &idle_t, &iowait, &irq, &softirq, &steal, &guest, &guest_nice
  );
  if (read_bytes != 10) {
    printf("/proc/stat not parsed!\n");
    fclose(stats_file);
    return -1;
  }

  fclose(stats_file);

  unsigned long total = user + nice + system + idle_t + iowait + irq + softirq + steal;
  unsigned long idle = idle_t + iowait;

  unsigned long total_diff = total - prev_total;
  unsigned long idle_diff = idle - prev_idle;
  
  //printf("%lu %lu\n", total_diff, idle_diff);
  double percentage = ((total_diff - idle_diff)*100.0) / total_diff;
  // double percentage = (total_diff - idle_diff) / total_diff;

  prev_total = total;
  prev_idle = idle;
  return percentage;
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
