#pragma once

#include <stdint.h>

int default_iface(char *iface);
int net_stat(const char *iface, uint64_t *rx, uint64_t *tx);
