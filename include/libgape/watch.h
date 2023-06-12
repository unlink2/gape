#ifndef WATCH_H_
#define WATCH_H_

#include <stddef.h>
#include <stdint.h>
#include "libgape/macros.h"

#define GAPE_NRUN_FOREVER -1
#define GAPE_SPIN_MS 100

struct GapeWatch;

/**
 * Callback function defs
 */

// runs a watch check and returns true whenever the watch has triggered
typedef bool (*GapeWatchCond)(struct GapeWatch *self);
// runs a watch action and retrns false if the watch should exit
typedef bool (*GapeWatchAct)(struct GapeWatch *self);

/**
 * Conditions
 */

// a test condition that always returns true
bool gape_watch_cond_true(struct GapeWatch *self);
// a test condition that always returns false
bool gape_watch_cond_false(struct GapeWatch *self);

/**
 * Acttions
 */

// an action that simply writes to a string buffer and exits after
// a single run
// watch_var should point to a char*
bool gape_watch_act_sprint(struct GapeWatch *self);

// Contains shared config for all action types
struct GapeWatch {
  GapeWatchCond cond;
  GapeWatchAct action;

  // number of runs before force exiting
  // if set to -1 run forever
  int32_t n_runs;

  // static watch config
  void *watch_cfg;
  // watch variables
  void *watch_var;
};

struct GapeWatch gape_watch_init(void);
struct GapeWatch gape_watch_true_init(GapeWatchAct act, void *watch_cfg,
                                      void *watch_var);
struct GapeWatch gape_watch_true_false(GapeWatchAct act, void *watch_cfg,
                                       void *watch_var);

void gape_watch(struct GapeWatch *self);

struct GapeWatch gape_watch_time_init(uint64_t seconds);

void gape_watch_free(struct GapeWatch *self);

#endif
