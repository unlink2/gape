#ifndef WATCH_H_
#define WATCH_H_

#include <stddef.h>
#include <stdint.h>
#include "libgape/macros.h"

struct GapeWatch;

// runs a watch check and returns true whenever the watch has triggered
typedef bool (*GapeWatchFn)(struct GapeWatch *self, char *dst);

// runs a watch action and retrns false if the watch should exit
typedef bool (*GapeWatchAction)(struct GapeWatch *self, char *dst);

// Contains shared config for all action types
struct GapeWatch {
  GapeWatchFn fn;
  GapeWatchAction action;
  // static watch config
  void *watch_cfg;
  // watch variables
  void *watch_var;
};

struct GapeWatch gape_watch_init(void);

struct GapeWatch gape_watch_time_init(uint64_t seconds);

void gape_watch_free(struct GapeWatch *self);

#endif
