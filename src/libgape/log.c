#include "libgape/log.h"

#include <stdint.h>

enum gape_loglevel GAPE_LOG_LEVEL;
FILE *GAPE_LOG_TARGET;

void gape_log_init(enum gape_loglevel level) {
  GAPE_LOG_LEVEL = level;
  GAPE_LOG_TARGET = stderr;
}
