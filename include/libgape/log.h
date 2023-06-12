#ifndef LOG_H_
#define LOG_H_

#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

enum GapeLogLevel {
  GAPE_LOG_LEVEL_OFF,
  GAPE_LOG_LEVEL_ERROR,
  GAPE_LOG_LEVEL_WARN,
  GAPE_LOG_LEVEL_INFO,
  GAPE_LOG_LEVEL_DBG,
};

void gape_log_init(enum GapeLogLevel level);

extern enum GapeLogLevel GAPE_LOG_LEVEL;
extern FILE *GAPE_LOG_TARGET;

#define gape_dbg_assert(a) assert((a))
#define gape_log_fprintf(f, level, ...)                                        \
  if ((level) <= LOG_LEVEL) {                                                  \
    fprintf((f), "[%d] ", level);                                              \
    fprintf((f), __VA_ARGS__);                                                 \
  }

#define gape_dbg(...) gape_log_fprintf(stderr, GAPE_LOG_LEVEL_DBG, __VA_ARGS__)
#define gape_warn(...)                                                         \
  gape_log_fprintf(stderr, GAPE_LOG_LEVEL_WARN, __VA_ARGS__)
#define gape_info(...)                                                         \
  gape_log_fprintf(stderr, GAPE_LOG_LEVEL_INFO, __VA_ARGS__)
#define gape_error(...)                                                        \
  gape_log_fprintf(stderr, GAPE_LOG_LEVEL_ERROR, __VA_ARGS__)

#define gape_panic(...)                                                        \
  {                                                                            \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(-1);                                                                  \
  }
#define gape_todo(...)                                                         \
  { gape_panic(__VA_ARGS__); }

#endif
