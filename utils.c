#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

// fsscanf - file string scan formated
int fsscanf (FILE *buff, const char *format, ...) {
  va_list args;
  char line[256];

  fseek(buff, 0, SEEK_SET); // move file pointer to 0 position (reset)

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
