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
// regular timer-based watch
bool gape_watch_cond_time(struct GapeWatch *self);

/**
 * Acttions
 */

// an action that simply writes to a string buffer and exits after
// a single run
// watch_var should point to a char*
bool gape_watch_act_sprint(struct GapeWatch *self);

// call execve
bool gape_watch_act_execve(struct GapeWatch *self);

// Contains shared config for all action types
struct GapeWatch {
  GapeWatchCond cond;
  GapeWatchAct act;

  // number of runs before force exiting
  // if set to -1 run forever
  int32_t n_runs;

  // watch cond config
  void *cond_cfg;

  // watch act config
  void *act_cfg;
};

struct GapeWatch gape_watch_init(void);

void gape_watch_set_cond(struct GapeWatch *self, GapeWatchCond cond, void *cfg);
void gape_watch_set_act(struct GapeWatch *self, GapeWatchAct act, void *cfg);

void gape_watch(struct GapeWatch *self);

void gape_watch_free(struct GapeWatch *self);

#endif
