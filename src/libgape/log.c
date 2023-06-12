#include "libgape/log.h"

#include <stdint.h>

enum GapeLogLevel GAPE_LOG_LEVEL;
FILE *GAPE_LOG_TARGET;

void gape_log_init(enum GapeLogLevel level) {
  GAPE_LOG_LEVEL = level;
  GAPE_LOG_TARGET = stderr;
}
