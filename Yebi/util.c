#include "env.h"
#include "util.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

void error(char *fmt, ...) {
  fprintf(stderr, "error: ");
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  print_env(stderr);
  exit(1);
}

