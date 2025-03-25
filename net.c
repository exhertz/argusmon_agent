#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

unsigned long net_stat(const char *iface, const char *stat) {
  char path[256];
  snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", iface, stat);

  FILE *fp = fopen(path, "r");
  if (!fp) return 0;

  unsigned long value = 0;
  fscanf(fp, "%lu", &value);
  fclose(fp);

  return value;
}

int default_iface(char *iface) {
  FILE *fp = fopen("/proc/net/route", "r");
  if (!fp) {
    perror("/proc/net/route/");
    return 1;
  }

  char line[256];
  unsigned long dest, gateway;
  fgets(line, sizeof(line), fp);

  while(fgets(line, sizeof(line), fp)) {
    if (sscanf(line, "%15s %lx %lx", iface, &dest, &gateway) == 3) {
      if (dest == 0) {
        printf("def interface: %s\n", iface);
        fclose(fp);
        return 0;
      }
    }
  }

  fclose(fp);
  return 1;
}

int main() {
  char iface[16];
  int resiface = default_iface(iface);
  if (!resiface) {
    printf("no return iface");
    return 1;
  }
  
  while (1) {
  static uint64_t prev_rx = 0;
  static uint64_t prev_tx = 0;
  uint64_t rx = net_stat(iface, "rx_bytes");
  uint64_t tx = net_stat(iface, "tx_bytes");

  uint64_t cur_rx = rx - prev_rx;
  uint64_t cur_tx = tx - prev_tx;

  prev_rx = rx;
  prev_tx = tx;
  printf("Download: %lu\n", cur_rx);
  printf("Upload: %lu\n", cur_tx);
  sleep(1);
  }
}

