#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "net.h"

uint64_t iface_stat(const char *iface, const char *stat) {
  char path[256];
  snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", iface, stat);

  FILE *fp = fopen(path, "r");
  if (!fp) {
    perror("file /sys/class/net/[interface]/statistics/[statistic] not opened");
    return 0;
  }

  uint64_t value = 0;
  fscanf(fp, "%lu", &value);
  fclose(fp);

  return value;
}

int default_iface(char *iface) {
  FILE *fp = fopen("/proc/net/route", "r");
  if (!fp) {
    perror("file /proc/net/route not opened");
    return -1;
  }

  char line[256];
  uint64_t dest, gateway;
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
  return -1;
}

int net_stat(const char *iface, uint64_t *rx, uint64_t *tx) {
  static uint64_t prev_rx = 0;
  static uint64_t prev_tx = 0;
  uint64_t rx_cur = iface_stat(iface, "rx_bytes");
  uint64_t tx_cur = iface_stat(iface, "tx_bytes");

  if (rx_cur < prev_rx || tx_cur < prev_tx) {
    prev_rx = prev_tx = 0;
    return -1;
  }

  uint64_t rx_diff = rx_cur - prev_rx;
  uint64_t tx_diff = tx_cur - prev_tx;

  prev_rx = rx_cur;
  prev_tx = tx_cur;

  *rx = rx_diff;
  *tx = tx_diff;

  return 0;
}
